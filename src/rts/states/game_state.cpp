#include "game_state.hpp"
#include "rts/states/game/lobby.hpp"
#include "rts/network/server.hpp"
#include "rts/states/game/command.hpp"

#include <SFGUI/SFGUI.hpp>
#include <iostream>

namespace
{
    const float move_speed = 5.0;
    const float zoom_speed = 1.2;
    const sf::Time turn_time = sf::milliseconds(200);
    const int millis_per_update = 1000 / 30;

    const int start_positions[rts::network::max_players][2] = {
        {0, 0}, {2, 2}, {2, 0}, {0, 2}, {1, 0}, {2, 1}, {1, 2}, {0, 1}
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
            m_update_time(sf::Time::Zero),
            m_player_turns(),
            m_state(CurrentState::InLobby),
            m_selecting(false),
            m_tile_dist(game::num_grass_tiles, game::num_tiles - 1),
            m_next_tile_sprite(context.texture_holder->get("tiles")),
            m_selected_sprite(context.texture_holder->get("select_arrow")),
            m_minion_counter_sprite(context.texture_holder->get("minion counter")),
            m_tile_placement_box_sprite(context.texture_holder->get("tile placement box")),
            m_hud_background_sprite(context.texture_holder->get("hud background")),
            m_minion_counter_text("0", context.font_holder->get("font")),
            m_minion_counter_shader(&context.shader_holder->get("minion"))
        {
            m_lobby = new game::Lobby(&m_lobby_done);
            m_lobby->add_to_desktop(*get_context().desktop);

            m_view = get_context().window->getView();
            m_selected_sprite.setOrigin(12, 71);
            m_minion_counter_sprite.setPosition(16, 170);
            m_minion_counter_text.setPosition(86, 180);
            m_next_tile_sprite.setOrigin(64, 64);

            get_context().texture_holder->get("hud background").setRepeated(true);
        }

        GameState::~GameState()
        {
            delete m_lobby;
            delete m_server;
        }

        void GameState::draw()
        {
            if(m_state != CurrentState::Playing) return;

            // draw tiles
            sf::VertexArray array(sf::Quads);
            for(int i = 0; i < m_size * m_size; ++i) {
                add_to_vertex_array(array, i);
            }

            get_context().window->draw(array, &get_context().texture_holder->get("tiles"));

            for(auto &tile : m_minion_tiles) {
                if(tile.empty()) continue;
                for(sf::Uint16 minion_id : tile) {
                    auto &minion = m_minions[minion_id];
                    if(minion.alive()) {
                        if(m_visibility.at(minion.get_x() / 128 + (minion.get_y() / 128) * m_size) == 2)
                            get_context().window->draw(minion);
                        if(minion.selected()) {
                            m_selected_sprite.setPosition(minion.get_x(), minion.get_y());
                            get_context().window->draw(m_selected_sprite);
                        }
                    }
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
            m_minion_counter_shader->setParameter("minion_colour", m_player_colours[m_my_player]);
            get_context().window->draw(m_minion_counter_sprite, m_minion_counter_shader);
            get_context().window->draw(m_minion_counter_text);

            int tu = m_next_tile.id % 8;
            int tv = m_next_tile.id / 8;
            if(!m_placing_tile)
                m_next_tile_sprite.setPosition(80, 80);
            m_next_tile_sprite.setRotation(90 * m_next_tile.orientation);
            m_next_tile_sprite.setTextureRect(sf::IntRect(tu * 128, tv * 128, 128, 128));
            get_context().window->draw(m_next_tile_sprite);

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
                } else if(event.type == sf::Event::MouseMoved) {
                    if(m_placing_tile) {
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
                                m_next_tile.id = m_tile_dist(m_tile_random);
                                m_next_tile.orientation = game::Tile::Orientation::NORTH;
                            }
                        }
                    }
                } else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right) {
                    if(m_placing_tile)
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
                    }
                } else if(event.type == sf::Event::KeyPressed) {
                    if(m_placing_tile) {
                        if(event.key.code == sf::Keyboard::Q) {
                            m_next_tile.orientation = (game::Tile::Orientation) ((m_next_tile.orientation + 3) % 4);
                        } else if(event.key.code == sf::Keyboard::E) {
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

                m_status_label = sfg::Label::Create("Waiting for other players (m/m)");
                sfg::Spinner::Ptr s = sfg::Spinner::Create();
                s->Start();
                sfg::Box::Ptr hbox = sfg::Box::Create(sfg::Box::HORIZONTAL);
                hbox->Pack(s);
                hbox->Pack(m_status_label);
                sfg::Window::Ptr w = sfg::Window::Create();
                w->SetTitle("Waiting...");
                w->Add(hbox);
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
            m_tile_random.seed(rd());
            m_next_tile.id = m_tile_dist(m_tile_random);

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

            m_minion_counter_text.setString(number_to_string(m_my_minions.size()));

            if(m_update_time > turn_time && m_finished_current_turn == false) {
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
                m_update_time -= turn_time;

                bool done = false;
                while(!done) {
                    game::Command command;
                    command = m_commands.get_command();

                    switch(command.type) {
                        case game::Command::COMMAND::MoveUnits:
                            for(auto minion : command.unit_move.to_move)
                                m_minions[minion].move_to(command.unit_move.x, command.unit_move.y);
                            break;
                        case game::Command::COMMAND::PlacePiece:
                            m_tiles[command.place_piece.x + command.place_piece.y * m_size] = command.place_piece.tile;
                            if(check_city(command.place_piece.x, command.place_piece.y)) {
                                std::cerr << "New city!!\n";
                            }
                            break;
                        case game::Command::COMMAND::Invalid:
                            done = true;
                            break;
                        default:
                            ASSERT(false);
                    }
                }
            }

            m_update_time += turn_time;
            update_input(dt);

            for(auto &v : m_minion_tiles)
                v.clear();
            std::fill(m_visibility.begin(), m_visibility.end(), 0);

            for(sf::Uint16 i = 0; i < m_minions.size(); ++i) {
                auto &minion = m_minions[i];
                sf::Uint16 x, y;
                x = minion.get_x() / 128;
                y = minion.get_y() / 128;

                if(minion.alive()) {
                    m_minion_tiles.at(get_id(x, y)).push_back(i);
                }

                if(minion.get_playerid() == m_my_player) {
                    sf::Uint16 view_distance;
                    sf::Uint16 fade_distance;
                    if(m_tiles[get_id(x, y)].has_watchtower()) {
                        fade_distance = 7;
                        view_distance = 5;
                    } else {
                        view_distance = 1;
                        fade_distance = 2;
                    }

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

            for(auto &minion : m_minions) {
                if(minion.alive())
                    minion.update(millis_per_update);
            }

            sf::Vector2f size = m_view.getSize();
            sf::Vector2f center = m_view.getCenter();

            if(center.x - size.x / 2.0 + 160 < 0)
                center.x = size.x / 2.0 - 160;
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

            std::uniform_int_distribution<sf::Uint8> hat_dist(0, 16);
            sf::Uint8 hatid = hat_dist(m_random);
            if(hatid >= 9)
                hatid = game::Minion::NO_HAT;
            game::Minion minion(player_num, hatid, m_player_colours[player_num], x, y, &get_context().texture_holder->get("minion"), &get_context().texture_holder->get("hats"), &get_context().shader_holder->get("minion"));

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

        bool GameState::check_city(sf::Uint16 x, sf::Uint16 y)
        {
            game::Tile &tile = get_tile(x, y);
            tile.check_time = m_commands.get_turn();
            bool possible_city = false;

            for(sf::Uint8 i = 0; i < 4; ++i) {
                if(tile.get_feature((game::Tile::Orientation) i) == game::Tile::EdgeFeature::CITY) {
                    possible_city = true;
                    if(!check_city(x, y, (game::Tile::Orientation)i))
                        return false;
                }
            }

            return possible_city;
        }

        bool GameState::check_city(sf::Uint16 x, sf::Uint16 y, game::Tile::Orientation direction)
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
            if(tile.check_time == m_commands.get_turn())
                return true;
            tile.check_time = m_commands.get_turn();

            for(game::Tile::Orientation dir : tile.connected_to(opp_direction))
                if(!check_city(x, y, dir))
                    return false;

            return true;
        }

        bool GameState::direction_is_valid(sf::Uint16 x, sf::Uint16 y, game::Tile::Orientation direction)
        {
            switch(direction) {
                case game::Tile::Orientation::NORTH:
                    return y != 0;
                case game::Tile::Orientation::EAST:
                    return x < m_size;
                case game::Tile::Orientation::SOUTH:
                    return y < m_size;
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
    }
}
