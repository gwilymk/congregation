/*
    Congregation - An rts game involving the placement of tiles
    Copyright (C) 2013  Gwilym Kuiper (kuiper.gwilym@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "game_state.hpp"
#include "rts/states/game/lobby.hpp"
#include "rts/network/server.hpp"
#include "rts/states/game/command.hpp"
#include "rts/tips.hpp"

#include <SFGUI/SFGUI.hpp>
#include <iostream>

#include <cstdlib>

namespace
{
    const sf::Time respawn_fade_time = sf::seconds(2);
    const float move_speed = 5.0;
    const float zoom_speed = 1.2;
    const int ticks_per_update = 5;
    const sf::Uint16 num_of_turns_per_minion_respawn = 400;
    const int millis_per_update = 1000 / 50;
    const sf::Uint32 death_probability = 10000000;

    const int start_positions[rts::network::max_players][2] = {
        {0, 0}, {2, 2}, {2, 0}, {0, 2}, {1, 0}, {2, 1}, {2, 2}, {0, 1}
    };

    const float s = 0.8, v = 0.8;
    
    sf::Color get_colour(float h)
    {
        h -= (long int)h;

        int h_i = h * 6;
        float f = h * 6.0 - h_i;
        float p = v * (1 - s);
        float q = v * (1 - f * s);
        float t = v * (1 - (1 - f) * s);

        float r = 0.0, g = 0.0, b = 0.0;
        
        if(h_i == 0) { r = v; g = t; b = p; }
        if(h_i == 1) { r = q; g = v; b = p; }
        if(h_i == 2) { r = p; g = v; b = t; }
        if(h_i == 3) { r = p; g = q; b = v; }
        if(h_i == 4) { r = t; g = p; b = v; }
        if(h_i == 5) { r = v; g = p; b = q; }

        return  sf::Color(r * 256, g * 256, b * 256);
    }

    template<typename T> T difference(T a, T b)
    {
        return (a > b) ? a - b : b - a;
    }
}

namespace rts
{
    namespace states
    {
        void GameState::add_to_vertex_array(sf::VertexArray &array, int id)
        {
            game::Tile tile = m_tiles[id];
            if(m_visibility[id] == 0)
                return;
            int tx = id % m_size;
            int ty = id / m_size;

            int tu = tile.id % 8;
            int tv = tile.id / 8;

            sf::Vertex v[4];

            v[0].position = sf::Vector2f(128 * tx, 128 * ty);
            v[1].position = sf::Vector2f(128 * (tx + 1), 128 * ty);
            v[2].position = sf::Vector2f(128 * (tx + 1), 128 * (ty + 1));
            v[3].position = sf::Vector2f(128 * tx, 128 * (ty + 1));

            v[(0 + tile.orientation) % 4].texCoords = sf::Vector2f(128 * tu, 128 * tv);
            v[(1 + tile.orientation) % 4].texCoords = sf::Vector2f(128 * (tu + 1), 128 * tv);
            v[(2 + tile.orientation) % 4].texCoords = sf::Vector2f(128 * (tu + 1), 128 * (tv + 1));
            v[(3 + tile.orientation) % 4].texCoords = sf::Vector2f(128 * tu, 128 * (tv + 1));

            for(auto &vertex : v) {
                if(m_visibility[id] == 1)
                    vertex.color = sf::Color(150, 150, 150, 150);
                array.append(vertex);
            }
        }

        GameState::GameState(StateStack &stack, Context context):
            State(stack, context),
            m_lobby_done(false),
            m_commands(),
            m_server(nullptr),
            m_channel(&m_lobby_done),
            m_player_turns(),
            m_lobby_background_sprite(context.texture_holder->get("lobby background")),
            m_state(CurrentState::InLobby),
            m_selecting(false),
            m_tile_dist(game::num_grass_tiles, game::Tile::get_total_probability() - 1),
            m_next_tile_sprite(context.texture_holder->get("tiles")),
            m_selected_sprite(context.texture_holder->get("select_arrow")),
            m_minion_counter_sprite(context.texture_holder->get("minion counter")),
            m_tile_placement_box_sprite(context.texture_holder->get("tile placement box")),
            m_hud_background_sprite(context.texture_holder->get("hud background")),
            m_minion_counter_text("0", context.font_holder->get("font")),
            m_minion_counter_shader(&context.shader_holder->get("minion")),
            m_minion_timer_sprite(context.texture_holder->get("clock background")),
            m_click_notification_sprite(context.texture_holder->get("click notification"), sf::IntRect(0, 0, 32, 32)),
            m_respawn_plus_sprite(context.texture_holder->get("respawn plus")),
            m_respawn_text("0", context.font_holder->get("font")),
            m_respawn_timer(respawn_fade_time)
        {
            m_lobby = new game::Lobby(&m_lobby_done);
            m_lobby->add_to_desktop(*get_context().desktop);

            m_view = get_context().window->getView();
            m_selected_sprite.setOrigin(12, 71);
            m_minion_counter_sprite.setPosition(16, 170);
            m_minion_counter_text.setPosition(86, 180);
            m_next_tile_sprite.setOrigin(64, 64);

            get_context().texture_holder->get("hud background").setRepeated(true);
            m_minion_timer_array.resize(num_of_turns_per_minion_respawn + 1);
            m_minion_timer_array[0].position = sf::Vector2f(80.0, 270.0);
            m_minion_timer_array[0].color = sf::Color(128, 226, 86);

            m_respawn_plus_sprite.setOrigin(16, 16);
            m_respawn_text.setOrigin(-20, 16);
            m_respawn_plus_sprite.setPosition(68, 196);
            m_respawn_text.setPosition(68, 196);

            m_minion_timer_sprite.setOrigin(64, 64);
            m_minion_timer_sprite.setPosition(80.0, 270.0);

            m_click_notification_sprite.setOrigin(16, 16);

            for(int i = 1; i < num_of_turns_per_minion_respawn; ++i) {
                m_minion_timer_array[i].position.x = 80.0 - 34.0 * std::sin((float)i * 2.0 * 3.1415926535 / num_of_turns_per_minion_respawn);
                m_minion_timer_array[i].position.y = 270.0 - 34.0 * std::cos((float)i * 2.0 * 3.1415926535 / num_of_turns_per_minion_respawn);
                m_minion_timer_array[i].color = sf::Color(128, 226, 86, 100);
            }
        }

        GameState::~GameState()
        {
            delete m_lobby;
            delete m_server;
        }

        void GameState::draw()
        {
            if(m_state != CurrentState::Playing) {
                get_context().window->draw(m_lobby_background_sprite);
                return;
            }

            // draw tiles
            sf::VertexArray array(sf::Quads);
            for(int i = 0; i < m_size * m_size; ++i) {
                add_to_vertex_array(array, i);
            }

            get_context().window->draw(array, &get_context().texture_holder->get("tiles"));
            sf::VertexArray minion_array(sf::Quads);
            sf::VertexArray hat_array(sf::Quads);

            for(auto &tile : m_minion_tiles) {
                if(tile.empty()) continue;
                for(sf::Uint16 minion_id : tile) {
                    auto &minion = m_minions[minion_id];
                    if(minion.alive()) {
                        if(m_visibility.at(minion.get_x() / 128 + (minion.get_y() / 128) * m_size) == 2)
                            minion.draw(minion_array, hat_array);
                    }
                }
            }

            sf::RenderStates states;
            states.texture = &get_context().texture_holder->get("minion");
            states.shader = m_minion_counter_shader;
            get_context().window->draw(minion_array, states);
            get_context().window->draw(hat_array, &get_context().texture_holder->get("hats"));

            for(sf::Uint16 mid : m_my_minions) {
                auto minion = m_minions[mid];
                if(minion.selected()) {
                    m_selected_sprite.setPosition(minion.get_x(), minion.get_y());
                    get_context().window->draw(m_selected_sprite);
                }
            }

            if(m_selecting) {
                sf::RectangleShape rect;
                rect.setPosition(m_select_start);
                rect.setSize(m_select_end - m_select_start);
                rect.setOutlineThickness(5);
                rect.setOutlineColor(sf::Color::Black);
                rect.setFillColor(sf::Color::Transparent);
                get_context().window->draw(rect);
            }

            sf::Vector2u window_size = get_context().window->getSize();
            sf::FloatRect view_rectf(0, 0, window_size.x, window_size.y);
            get_context().window->setView(sf::View(view_rectf));

            sf::IntRect view_recti(0, 0, m_hud_background_sprite.getLocalBounds().width, window_size.y);
            m_hud_background_sprite.setTextureRect(view_recti);
            get_context().window->draw(m_hud_background_sprite);
            get_context().window->draw(m_tile_placement_box_sprite);
            m_minion_counter_sprite.setColor(m_player_colours[m_my_player]);
            get_context().window->draw(m_minion_counter_sprite, m_minion_counter_shader);
            get_context().window->draw(m_minion_counter_text);

            int tu = m_next_tile.id % 8;
            int tv = m_next_tile.id / 8;
            if(!m_placing_tile)
                m_next_tile_sprite.setPosition(80, 80);
            m_next_tile_sprite.setRotation(90 * m_next_tile.orientation);
            m_next_tile_sprite.setTextureRect(sf::IntRect(tu * 128, tv * 128, 128, 128));
            get_context().window->draw(m_next_tile_sprite);

            if(m_respawn_timer < respawn_fade_time) {
                float factor = m_respawn_timer.asSeconds() / respawn_fade_time.asSeconds();
                int y = 196 - factor * 32.0;
                int x = 63;

                m_respawn_plus_sprite.setPosition(x, y);
                m_respawn_text.setPosition(x, y);
                m_respawn_plus_sprite.setColor(sf::Color(255, 255, 255, (1.0 - factor) * 255.0));
                m_respawn_text.setColor(sf::Color(255, 255, 255, (1.0 - factor) * 255.0));
                get_context().window->draw(m_respawn_plus_sprite);
                get_context().window->draw(m_respawn_text);
            }

            unsigned int num_verts_for_timer = num_of_turns_per_minion_respawn - (m_commands.get_turn() % num_of_turns_per_minion_respawn);
            get_context().window->draw(m_minion_timer_sprite);
            get_context().window->draw(m_minion_timer_array.data(), num_verts_for_timer, sf::TrianglesFan);

            if(m_click_frame != -1) {
                m_click_notification_sprite.setTextureRect(sf::IntRect(m_click_frame * 32, 0, 32, 32));
                get_context().window->draw(m_click_notification_sprite);
            }

            get_context().window->setView(m_view);
        }

        bool GameState::update(sf::Time dt)
        {
            get_context().desktop->Update(dt.asSeconds());
            switch(m_state) {
                case CurrentState::InLobby:
                    lobby_update(dt);
                    break;
                case CurrentState::Waiting:
                    waiting_update(dt);
                    break;
                case CurrentState::SettingUp:
                    setting_up_update(dt);
                    break;
                case CurrentState::Playing:
                    playing_update(dt);
                    break;
            }

            return true;
        }

        bool GameState::handle_event(const sf::Event &event)
        {
            if(event.type == sf::Event::Closed)
                request_stack_pop();

            get_context().desktop->HandleEvent(event);

            if(m_state == CurrentState::Playing) {
                if(event.type == sf::Event::MouseWheelMoved) {
                    int delta = event.mouseWheel.delta;
                    if(delta >= 0) {
                        m_view.zoom(1 / (delta * zoom_speed));
                    } else {
                        m_view.zoom(-delta * zoom_speed);
                    }

                    sf::Vector2f view_size = m_view.getSize();
                    sf::Vector2u window_size = get_context().window->getSize();

                    if((float)window_size.x / view_size.x < 0.5) {
                        view_size.x = window_size.x * 2.0;
                        view_size.y = window_size.y * 2.0;
                        m_view.setSize(view_size);
                    }
                } else if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left && !m_placing_tile) {
                    if(event.mouseButton.x > 160) {
                        m_select_end = m_select_start = get_context().window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));                   
                        m_selecting = true;
                    }
                } else if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Right) {
                    m_last_mouse_position = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                } else if(event.type == sf::Event::MouseMoved) {
                    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                        sf::Vector2i pos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);

                        if(m_moving_view) {
                            sf::Vector2i delta = pos - m_last_mouse_position;
                            if(delta.x || delta.y) {
                                m_view.move(static_cast<sf::Vector2f>(delta));
                                sf::Mouse::setPosition(m_last_mouse_position, *get_context().window);
                            }
                        } else {
                            sf::Vector2i delta = pos  - m_last_mouse_position;
                            if(delta.x * delta.x + delta.y * delta.y > 20 * 20)
                                m_moving_view = true;
                        }
                    } else if(m_placing_tile) {
                        m_next_tile_sprite.setPosition(event.mouseMove.x, event.mouseMove.y);
                    } else if(m_selecting) {
                        m_select_end = get_context().window->mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                    }
                } else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left && !m_placing_tile) {
                    sf::Vector2f positionf(event.mouseButton.x, event.mouseButton.y);

                    if(event.mouseButton.x <= 160) {
                        if(!m_placing_tile && !m_selecting) {
                            if(m_next_tile_sprite.getLocalBounds().contains(positionf)) {
                                m_placing_tile = true;
                            }
                        } else {
                            m_placing_tile = false;
                        }
                    } else {
                        if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                            for(auto &minion : m_my_minions)
                                m_minions[minion].deselect();

                        sf::Vector2i mousecoords(event.mouseButton.x, event.mouseButton.y);
                        m_select_end = get_context().window->mapPixelToCoords(mousecoords);

                        if(m_select_start.x < m_select_end.x) {
                            if(m_select_start.y > m_select_end.y) {
                                sf::Vector2f tmp1, tmp2;
                                tmp1.x = m_select_start.x;
                                tmp1.y = m_select_end.y;
                                tmp2.x = m_select_end.x;
                                tmp2.y = m_select_start.y;

                                m_select_start = tmp1;
                                m_select_end = tmp2;
                            }
                        } else {
                            if(m_select_start.y > m_select_end.y) {
                                std::swap(m_select_start, m_select_end);
                            } else {
                                sf::Vector2f tmp1, tmp2;
                                tmp1.x = m_select_start.x;
                                tmp1.y = m_select_end.y;
                                tmp2.x = m_select_end.x;
                                tmp2.y = m_select_start.y;

                                m_select_start = tmp2;
                                m_select_end = tmp1;
                            }
                        }

                        sf::Vector2f size = m_select_end - m_select_start;
                        sf::FloatRect rect = sf::FloatRect(m_select_start, size);
                        bool rectangle_select = size.x * size.x + size.y * size.y > 10;

                        for(auto minion : m_my_minions) {
                            if(rectangle_select) {
                                if(rect.intersects(m_minions[minion].get_bounds()))
                                    m_minions[minion].toggle_selection();
                            } else {
                                if(m_minions[minion].get_bounds().contains(m_select_start))
                                    m_minions[minion].toggle_selection();
                            }
                        }

                        m_selecting = false;
                    }
                } else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left && m_placing_tile) {
                    if(event.mouseButton.x <= 160)
                        m_placing_tile = false;
                    else {
                        sf::Vector2f target = get_context().window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                        game::Command command;
                        command.type = game::Command::COMMAND::PlacePiece;
                        command.place_piece.x = target.x / 128;
                        command.place_piece.y = target.y / 128;
                        if(command.place_piece.x < m_size && command.place_piece.y < m_size) {
                            command.place_piece.tile = m_next_tile;
                            unsigned position_id = command.place_piece.x + command.place_piece.y * m_size;
                            if(m_visibility[position_id] == 2 && m_tiles[position_id].id < game::num_grass_tiles && legal_move(m_next_tile, command.place_piece.x, command.place_piece.y)) {
                                send_command(command);
                                m_placing_tile = false;
                                m_next_tile.id = game::Tile::get_tile(m_tile_dist(m_tile_random));
                                m_next_tile.orientation = game::Tile::Orientation::NORTH;
                            }
                        }
                    }
                } else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right) {
                    if(m_moving_view)
                        m_moving_view = false;
                    else if(m_placing_tile)
                        m_placing_tile = false;
                    else if(event.mouseButton.x > 160) {
                        game::Command command;
                        command.type = game::Command::COMMAND::MoveUnits;
                        for(auto minion : m_my_minions) {
                            if(m_minions[minion].selected()) {
                                command.unit_move.to_move.push_back(minion);
                            }
                        }

                        if(!command.unit_move.to_move.empty()) {
                            sf::Vector2f target = get_context().window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                            if(target.x > 0 && target.x < m_size * 128 && target.y > 0 && target.y < m_size * 128) {
                                command.unit_move.x = target.x;
                                command.unit_move.y = target.y;
                                send_command(command);
                            }
                        }

                        m_click_notification_sprite.setPosition(event.mouseButton.x, event.mouseButton.y);
                        m_click_frame = 0;
                    }
                } else if(event.type == sf::Event::KeyPressed) {
                    if(m_placing_tile) {
                        if(event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Quote || event.key.code == sf::Keyboard::Unknown) {
                            m_next_tile.orientation = (game::Tile::Orientation) ((m_next_tile.orientation + 3) % 4);
                        } else if(event.key.code == sf::Keyboard::E || event.key.code == sf::Keyboard::Period) {
                            m_next_tile.orientation = (game::Tile::Orientation) ((m_next_tile.orientation + 1) % 4);
                        }
                    }
                }
            }
            return true;
        }

        void GameState::lobby_update(sf::Time dt)
        {
            if(!m_lobby_done)
                m_lobby->update(dt);

            if(m_lobby_done) {
                if(m_lobby->server()) {
                    m_server = new network::Server(m_lobby->get_server_info());
                    m_server->listen();
                    m_channel.connect_to_server(sf::IpAddress::LocalHost, m_server->port());
                } else
                    m_channel.connect_to_server(m_lobby->get_server(), m_lobby->get_server_port());


                delete m_lobby;
                m_lobby = nullptr;
                m_lobby_done = false;

                m_state = CurrentState::Waiting;

                sfg::Box::Ptr vbox = sfg::Box::Create(sfg::Box::VERTICAL);
                m_status_label = sfg::Label::Create("Waiting for other players (m/m)");
                sfg::Spinner::Ptr s = sfg::Spinner::Create();
                s->Start();
                sfg::Box::Ptr hbox = sfg::Box::Create(sfg::Box::HORIZONTAL);
                hbox->Pack(s);
                vbox->Pack(m_status_label);
                vbox->Pack(sfg::Label::Create(tips[std::time(NULL) % num_tips]));
                hbox->Pack(vbox);
                sfg::Window::Ptr w = sfg::Window::Create();
                w->SetTitle("Waiting...");
                w->Add(hbox);
                w->SetPosition(sf::Vector2f(100, 100));
                get_context().desktop->Add(w);
            }
        }

        void GameState::waiting_update(sf::Time)
        {
            if(m_lobby_done) {
                delete m_server;
                m_server = nullptr;
                m_state = CurrentState::SettingUp;
                get_context().desktop->RemoveAll();
                m_status_label.reset();
                m_info = m_channel.get_server_info();
                return;
            }

            m_channel.update();

            std::ostringstream ss;
            ss << "Waiting for other players (" << m_channel.num_players() << '/' << m_channel.max_players() << ')';
            m_status_label->SetText(ss.str());
        }

        void GameState::setting_up_update(sf::Time)
        {
            m_my_player = m_channel.get_my_player();

            std::array<sf::Uint32, network::seed_size> seed = m_channel.get_seed();
            std::seed_seq seq(seed.begin(), seed.end());
            m_random.seed(seq);

            sf::Uint8 map_size = m_info.map_size;
            m_size = network::map_sizes[map_size];

            m_tiles.reserve(m_size * m_size);
            std::geometric_distribution<sf::Uint8> grass_dist(0.75);
            std::uniform_int_distribution<sf::Uint8> orientation_dist(game::Tile::Orientation::NORTH, game::Tile::Orientation::WEST + 1);

            for(int i = 0 ; i < m_size * m_size ; ++i) {
                sf::Uint8 tileid = grass_dist(m_random);
                if(tileid >= game::num_grass_tiles) tileid = game::num_grass_tiles - 1;
                m_tiles[i] = game::Tile(tileid, (game::Tile::Orientation)orientation_dist(m_random));
            }

            m_state = CurrentState::Playing;

            std::uniform_real_distribution<float> colour_dist(0, 1);
            float h = colour_dist(m_random);
            m_player_colours.reserve(m_channel.num_players());
            for(int i = 0; i < m_info.max_players; ++i) {
                m_player_colours[i] = get_colour(h);
                h += 0.618033;
                h -= (int)h;
            }

            for(int i = 0; i < m_channel.num_players(); ++i) {
                sf::Uint16 x_coord = 0;
                sf::Uint16 y_coord = 0;

                if(start_positions[i][0] == 0) x_coord = 128 * 4;
                else if(start_positions[i][0] == 1) x_coord = m_size / 2;
                else if(start_positions[i][0] == 2) x_coord = m_size * 128 - 128 * 4;
                else ASSERT(false);

                if(start_positions[i][1] == 0) y_coord = 128 * 4;
                else if(start_positions[i][1] == 1) y_coord = m_size / 2;
                else if(start_positions[i][1] == 2) y_coord = m_size * 128 - 128 * 4;
                else ASSERT(false);

                x_coord += 64;
                y_coord += 64;

                create_minion(x_coord, y_coord, i);

                if(i == m_my_player)
                    m_view.setCenter(x_coord, y_coord);
            }

            std::random_device rd;
            m_tile_random.seed(std::time(NULL));
            m_next_tile.id = game::Tile::get_tile(m_tile_dist(m_tile_random));

            for(int i = 0; i < m_channel.num_players(); ++i)
                m_player_turns.push_back(0);
            std::cerr << "Started a game for " << m_channel.num_players() << " players\n";

            m_visibility.resize(m_size * m_size);
            m_minion_tiles.resize(m_size * m_size);
        }

        void GameState::playing_update(sf::Time dt)
        {
            sf::Packet packet;
            sf::Uint8 playerid;

            if(m_click_frame != -1) {
                m_click_timer += dt;
                if(m_click_timer > sf::seconds(1 / 30.0f)) {
                    m_click_timer -= sf::seconds(1 / 30.0f);
                    ++m_click_frame;
                    if(m_click_frame >= 5)
                        m_click_frame = -1;
                }
            }

            m_respawn_timer += dt;

            m_minion_counter_text.setString(number_to_string(m_my_minions.size()));

            if(num_ticks >= ticks_per_update && m_finished_current_turn == false) {
                m_finished_current_turn = true;
                packet.clear();
                packet << network::next_turn;
                packet << m_commands.get_turn();
                sf::Socket::Status status = m_channel.send(packet);
                ASSERT(status == sf::Socket::Done);
            }

            while(m_channel.receive(packet, playerid) == sf::Socket::Done) {
                std::string message;
                packet >> message;
                if(message == network::next_turn) {
                    sf::Uint32 turn_num;
                    packet >> turn_num;
                    m_player_turns[playerid] = turn_num;
                    if(turn_num > m_commands.get_turn() + game::num_turns_to_store) {
                        std::cerr << "Player " << (unsigned) playerid << " is on turn " << turn_num << " but I'm on turn " << (unsigned) m_commands.get_turn() << "!\n";
                        ASSERT(false);
                    }
                } else if(message == network::add_command) {
                    game::Command command;
                    packet >> command;
                    m_commands.add_command(command);
                } else {
                    std::cerr << "====================================================================\n";
                    std::cerr << "==============    Got a false command ==============================\n";
                    std::cerr << "============== " << message << " ===================================\n";
                    ASSERT(false);
                }

                packet.clear();
            }

            if(m_finished_current_turn) {
                for(unsigned int i = 0; i < m_player_turns.size(); ++i) {
                    if(m_player_turns[i] + game::num_turns_to_store - 1 <= m_commands.get_turn()) {
                        std::cerr << "Player " << i << " is holding the game up ";
                        for(auto i : m_player_turns) std::cerr << i << " ";
                        std::cerr << std::endl;
                        return;
                    }
                }

                m_commands.next_turn();
                m_player_turns[m_my_player] = m_commands.get_turn();
                m_finished_current_turn = false;
                num_ticks = 0;

                bool done = false;
                while(!done) {
                    game::Command command;
                    command = m_commands.get_command();

                    switch(command.type) {
                        case game::Command::COMMAND::MoveUnits:
                            for(auto minion : command.unit_move.to_move)
                                m_minions[minion].move_to(command.unit_move.x, command.unit_move.y, m_tiles, m_size);
                            break;
                        case game::Command::COMMAND::PlacePiece:
                            m_tiles[command.place_piece.x + command.place_piece.y * m_size] = command.place_piece.tile;
                            break;
                        case game::Command::COMMAND::Invalid:
                            done = true;
                            break;
                        default:
                            ASSERT(false);
                    }
                }
            }

            ++num_ticks;
            update_input(dt);

            for(auto &v : m_minion_tiles)
                v.clear();
            std::fill(m_visibility.begin(), m_visibility.end(), 0);

            for(sf::Uint16 i = 0; i < m_minions.size(); ++i) {
                auto &minion = m_minions[i];
                if(!minion.alive())
                    continue;
                sf::Uint16 x, y;
                x = minion.get_x() / 128;
                y = minion.get_y() / 128;

                m_minion_tiles[get_id(x, y)].push_back(i);

                if(minion.get_playerid() == m_my_player) {
                    sf::Uint16 view_distance;
                    sf::Uint16 fade_distance;
                    if(m_tiles[get_id(x, y)].has_watchtower()) {
                        fade_distance = 5;
                        view_distance = 4;
                    } else {
                        view_distance = 1;
                        fade_distance = 2;
                    }
                    
                    light_up(x, y, view_distance, fade_distance);
                }
                
                game::Tile &tile = get_tile(x, y);
                game::Tile::Orientation direction = tile.get_direction(minion.get_x() % 128, minion.get_y() % 128);
                if(tile.get_feature(direction) == game::Tile::CITY) {
                    if(minion.get_playerid() == m_my_player) {
                        check_city(x, y, direction, ++m_counter, [this] (sf::Uint16 x, sf::Uint16 y, game::Tile::Orientation) { light_up(x, y, 1, 2); });
                        for(auto dir : tile.connected_to(direction)) {
                            check_city(x, y, dir, ++m_counter, [this] (sf::Uint16 x, sf::Uint16 y, game::Tile::Orientation) { light_up(x, y, 1, 2); });
                        }
                    }
                }
            }

            if(m_commands.get_turn() % num_of_turns_per_minion_respawn == 0 && !m_minions_have_spawned) {
                int num_my_minions_respawned = 0;
                m_minions_have_spawned = true;
                
                std::deque<std::pair<sf::Uint16, sf::Uint8>> spawn_queue;

                for(auto &minion : m_minions) {
                    if(!minion.alive())
                        continue;
                    if(minion.check_city_turn == m_commands.get_turn())
                        continue;
                    sf::Uint16 x = minion.get_x() / 128, y = minion.get_y() / 128;
                    game::Tile &tile = get_tile(x, y);
                    game::Tile::Orientation direction = tile.get_direction(minion.get_x() % 128, minion.get_y() % 128);
                    if(tile.get_feature(direction) == game::Tile::CITY) {
                        bool should_spawn = true;
                        std::set<sf::Uint16> checked_ids;
                        auto func = [&] (sf::Uint16 tile_x, sf::Uint16 tile_y, game::Tile::Orientation dir) {
                            sf::Uint16 id = get_id(tile_x, tile_y);
                            for(sf::Uint16 m : m_minion_tiles[id]) {
                                if(m_tiles[id].get_direction(m_minions[m].get_x() % 128, m_minions[m].get_y() % 128) == dir) {
                                    if(m_minions[m].get_playerid() != minion.get_playerid()) {
                                        should_spawn = false;
                                        m_minions[m].check_city_turn = m_commands.get_turn();
                                    } else if(m_minions[m].check_city_turn == m_commands.get_turn()) {
                                        should_spawn = false;
                                        m_minions[m].check_city_turn = m_commands.get_turn();
                                    } 
                                }
                            }
                            checked_ids.insert(id);
                        };

                        bool completed_city = check_city(x, y, direction, ++m_counter);
                        for(auto dir : tile.connected_to(direction)) {
                            if(!check_city(x, y, dir, ++m_counter)) {
                                completed_city = false;
                            }
                        }

                        if(completed_city) {
                            func(x, y, direction);
                            check_city(x, y, direction, ++m_counter, func);
                            for(auto dir : tile.connected_to(direction)) {
                                func(x, y, dir);
                                check_city(x, y, dir, ++m_counter, func);
                            }

                            if(should_spawn) {
                                for(sf::Uint16 id : checked_ids) {
                                    if(id != get_id(x, y))
                                        spawn_queue.push_back(std::make_pair(id, minion.get_playerid()));
                                }

                                if(minion.get_playerid() == m_my_player)
                                    num_my_minions_respawned += checked_ids.size() - 1;
                            } 
                        }
                        
                        minion.check_city_turn = m_commands.get_turn();
                    }
                }

                for(auto pair : spawn_queue) {
                    sf::Uint16 x = pair.first % m_size;
                    sf::Uint16 y = pair.first / m_size;

                    create_minion(x * 128 + 64, y * 128 + 64, pair.second);
                }

                if(num_my_minions_respawned) {
                    m_respawn_text.setString(number_to_string(num_my_minions_respawned));
                    m_respawn_timer = sf::Time::Zero;
                }
            }

            if(m_commands.get_turn() % num_of_turns_per_minion_respawn)
                m_minions_have_spawned = false;

            for(auto &minion : m_minions) {
                if(minion.alive())
                    minion.update(millis_per_update, m_tiles);
            }

            for(auto &minion_list : m_minion_tiles) {
                std::vector<std::pair<sf::Uint16, sf::IntRect>> collisions;
                for(sf::Uint16 minion : minion_list) {
                    collisions.emplace_back(minion, m_minions[minion].get_collision_bounds());
                }
                sf::Uint16 size = collisions.size();
                for(sf::Uint16 i = 0; i < size; ++i) {
                    for(sf::Uint16 j = i + 1; j < size; ++j) {
                        sf::Uint16 idi = collisions[i].first, idj = collisions[j].first;

                        auto &mi = m_minions[idi];
                        auto &mj = m_minions[idj];
                        if(!mi.alive() || !mj.alive()) continue;
                        sf::Uint16 disth = difference(mi.get_collision_bounds().left, mj.get_collision_bounds().left);
                        sf::Uint16 distv = difference(mi.get_collision_bounds().top, mj.get_collision_bounds().top);

                        if(collisions[i].second.intersects(collisions[j].second)) {
                            if(mi.get_playerid() == mj.get_playerid()) {
                                bool direction; // vertical = true, horizontal = false
                                if(distv > disth)
                                    direction = true;
                                else if(distv < disth)
                                    direction = false;
                                else 
                                    direction = m_random() > m_random.max() / 2;

                                if(direction) {
                                    if(mi.get_collision_bounds().top > mj.get_collision_bounds().top) {
                                        if(mi.get_y() < m_size * 128 - 1 && !mi.is_moving())
                                            mi.set_y(mi.get_y() + 1);
                                        if(mj.get_y() != 0 && !mj.is_moving())
                                            mj.set_y(mj.get_y() - 1);
                                    } else {
                                        if(mj.get_y() < m_size * 128 - 1 && !mj.is_moving())
                                            mj.set_y(mj.get_y() + 1);
                                        if(mi.get_y() != 0 && !mi.is_moving())
                                            mi.set_y(mi.get_y() - 1);
                                    }
                                } else {
                                    if(mi.get_collision_bounds().left > mj.get_collision_bounds().left) {
                                        if(mi.get_x() < m_size * 128 - 1 && !mi.is_moving())
                                            mi.set_x(mi.get_x() + 1);
                                        if(mj.get_x() != 0 && !mj.is_moving())
                                            mj.set_x(mj.get_x() - 1);
                                    } else {
                                        if(mj.get_x() < m_size * 128 - 1 && !mj.is_moving())
                                            mj.set_x(mj.get_x() + 1);
                                        if(mi.get_x() != 0 && !mi.is_moving())
                                            mi.set_x(mi.get_x() - 1);
                                    }
                                }
                            } else {
                                mi.set_action(game::Minion::Action::FIGHTING);
                                mj.set_action(game::Minion::Action::FIGHTING);

                                if(m_random() < death_probability)
                                    kill_minion(idi);
                                if(m_random() < death_probability)
                                    kill_minion(idj);
                            }
                        }
                    }
                }
            }

            sf::Vector2f size = m_view.getSize();
            sf::Vector2f center = m_view.getCenter();

            if(center.x - size.x / 2.0 + 160.0 * size.x / get_context().window->getSize().x < 0)
                center.x = size.x / 2.0 - 160.0 * size.x / get_context().window->getSize().x;
            if(center.y - size.y / 2.0 < 0)
                center.y = size.y / 2.0;
            if(center.x + size.x / 2.0 > m_size * 128)
                center.x = m_size * 128 - size.x / 2.0;
            if(center.y + size.y / 2.0 > m_size * 128)
                center.y = m_size * 128 - size.y / 2.0;

            m_view.setCenter(center);
            get_context().window->setView(m_view);
        }

        void GameState::update_input(sf::Time dt)
        {
            if(!*get_context().focused)
                return;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::K))
                m_view.move(0, -dt.asSeconds() * 128.0 * move_speed);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::H))
                m_view.move(-dt.asSeconds() * 128.0 * move_speed, 0);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::J))
                m_view.move(0, dt.asSeconds() * 128.0 * move_speed);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::L))
                m_view.move(dt.asSeconds() * 128.0 * move_speed, 0);
        }

        void GameState::create_minion(sf::Uint16 x, sf::Uint16 y, sf::Uint8 player_num)
        {
            ASSERT(x > 0 && x < m_size * 128);
            ASSERT(y > 0 && y < m_size * 128);

            std::uniform_int_distribution<sf::Uint8> hat_dist(0, game::Minion::NUM_HATS - 1 + game::Minion::NUM_HATS);
            sf::Uint8 hatid = hat_dist(m_random);
            if(hatid >= game::Minion::NUM_HATS)
                hatid = game::Minion::NO_HAT;
            game::Minion minion(player_num, hatid, m_player_colours[player_num], x, y, m_size);

            sf::Uint16 minionid;

            if(!m_free_list.empty()) {
                auto itr = m_free_list.begin();
                m_minions[*itr] = minion;
                minionid = *itr;
                m_free_list.erase(itr);
            } else {
                minionid = m_minions.size();
                minion.set_direction(minionid % 4);
                m_minions.push_back(minion);
            }

            if(player_num == m_my_player)
                m_my_minions.push_back(minionid);
        }

        void GameState::kill_minion(sf::Uint16 id)
        {
            if(m_minions[id].get_playerid() == m_my_player) {
                m_my_minions.erase(std::find(m_my_minions.begin(), m_my_minions.end(), id));
            }

            m_minions[id].kill();
            m_free_list.insert(id);
        }

        void GameState::send_command(game::Command command)
        {
            command.turn = m_commands.get_turn() + game::num_turns_to_store;

            m_commands.add_command(command);
            sf::Packet packet;
            packet << network::add_command;
            packet << command;
            ASSERT(m_channel.send(packet) == sf::Socket::Done);
        }

        static bool check_direction(game::Tile tileA, game::Tile tileB, game::Tile::Orientation direction)
        {
            if(!(tileA.get_feature(direction) == tileB.get_feature((game::Tile::Orientation) ((direction + 2) % 4)) || tileB.id < game::num_grass_tiles)) {
                std::cerr << "Cannot place " << (unsigned) tileA.id << " with orientation " << (unsigned) tileA.orientation << " because its " << tileA.get_feature(direction) << " collides with " << (unsigned) tileB.id << "'s feature " << (unsigned)tileB.get_feature((game::Tile::Orientation) ((direction + 2) % 4)) << std::endl;
                return false;
            } else {
                return true;
            }
        }

        bool GameState::legal_move(game::Tile tile, sf::Uint16 x, sf::Uint16 y)
        {
            if(x > 0) 
                if(!check_direction(tile, get_tile(x - 1, y), game::Tile::Orientation::WEST))
                    return false;
            if(x < m_size - 1)
                if(!check_direction(tile, get_tile(x + 1, y), game::Tile::Orientation::EAST))
                    return false;
            if(y > 0) 
                if(!check_direction(tile, get_tile(x, y - 1), game::Tile::Orientation::NORTH))
                    return false;
            if(y < m_size - 1)
                if(!check_direction(tile, get_tile(x, y + 1), game::Tile::Orientation::SOUTH))
                    return false;
            return true;
        }

        game::Tile &GameState::get_tile(sf::Uint16 x, sf::Uint16 y)
        {
            return m_tiles[get_id(x, y)];
        }

        bool GameState::check_city(sf::Uint16 x, sf::Uint16 y, game::Tile::Orientation direction, sf::Uint32 time, std::function<void(sf::Uint16, sf::Uint16, game::Tile::Orientation direction)> function)
        {
            if(!direction_is_valid(x, y, direction))
                return false;

            switch(direction) {
                case game::Tile::Orientation::NORTH:
                    --y;
                    break;
                case game::Tile::Orientation::EAST:
                    ++x;
                    break;
                case game::Tile::Orientation::SOUTH:
                    ++y;
                    break;
                case game::Tile::Orientation::WEST:
                    --x;
                    break;
            }

            game::Tile &tile = get_tile(x, y);
            game::Tile::Orientation opp_direction = (game::Tile::Orientation) ((direction + 2) % 4);
            if(tile.get_feature(opp_direction) != game::Tile::EdgeFeature::CITY)
                return false;
            if(tile.check_time == time)
                return tile.cache;
            tile.check_time = time;

            tile.cache = true;

            if(function)
                function(x, y, opp_direction);

            for(game::Tile::Orientation dir : tile.connected_to(opp_direction)) {
                if(function)
                    function(x, y, dir);
                if(!check_city(x, y, dir, time, function)) {
                    tile.cache = false;
                }
            }

            return tile.cache;
        }

        bool GameState::direction_is_valid(sf::Uint16 x, sf::Uint16 y, game::Tile::Orientation direction)
        {
            switch(direction) {
                case game::Tile::Orientation::NORTH:
                    return y != 0;
                case game::Tile::Orientation::EAST:
                    return x < m_size - 1;
                case game::Tile::Orientation::SOUTH:
                    return y < m_size - 1;
                case game::Tile::Orientation::WEST:
                    return x != 0;
                default:
                    return false;
            }
        }

        game::Tile &GameState::tile_in_direction(sf::Uint16 x, sf::Uint16 y, game::Tile::Orientation direction)
        {
            ASSERT(direction_is_valid(x, y, direction));

            switch(direction) {
                case game::Tile::Orientation::NORTH:
                    return get_tile(x, y - 1);
                case game::Tile::Orientation::EAST:
                    return get_tile(x + 1, y);
                case game::Tile::Orientation::SOUTH:
                    return get_tile(x, y + 1);
                case game::Tile::Orientation::WEST:
                    return get_tile(x - 1, y);
                default:
                    return get_tile(-1, -1);
            }
        }

        sf::Uint16 GameState::get_id(sf::Uint16 x, sf::Uint16 y) const
        {
            ASSERT(x < m_size && y < m_size);
            return x + y * m_size;
        }

        void GameState::light_up(sf::Uint16 x, sf::Uint16 y, sf::Uint16 view_distance, sf::Uint16 fade_distance)
        {
            sf::Uint16 fade_dist_sq = fade_distance * fade_distance;
            sf::Uint16 view_dist_sq = view_distance * view_distance;

            for(sf::Uint16 i = x - fade_distance; i != x + fade_distance + 1; ++i) {
                for(sf::Uint16 j = y - fade_distance; j != y + fade_distance + 1; ++j) {
                    if(i >= m_size || j >= m_size)
                        continue;

                    sf::Uint16 dist_sq = (i - x) * (i - x) + (j - y) * (j - y);
                    if(dist_sq <= view_dist_sq)
                        m_visibility[get_id(i, j)] = 2;
                    else if(dist_sq <= fade_dist_sq) 
                        m_visibility[get_id(i, j)] = std::max(m_visibility[get_id(i, j)], (sf::Uint8)1);
                }
            }
        }
    }
}
