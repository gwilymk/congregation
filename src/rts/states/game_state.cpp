#include "game_state.hpp"
#include "rts/states/game/lobby.hpp"
#include "rts/network/server.hpp"

#include <SFGUI/SFGUI.hpp>

namespace
{
    const float move_speed = 5.0;
    const float zoom_speed = 1.2;

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
        void GameState::add_to_vertex_array(sf::VertexArray &array, const game::Tile &tile, int id)
        {
            int tx = id % m_size;
            int ty = id / m_size;

            int tu = tile.id % 8;
            int tv = tile.id / 8;

            array[id * 4 + 0].position = sf::Vector2f(128 * tx, 128 * ty);
            array[id * 4 + 1].position = sf::Vector2f(128 * (tx + 1), 128 * ty);
            array[id * 4 + 2].position = sf::Vector2f(128 * (tx + 1), 128 * (ty + 1));
            array[id * 4 + 3].position = sf::Vector2f(128 * tx, 128 * (ty + 1));

            array[id * 4 + (0 + tile.orientation) % 4].texCoords = sf::Vector2f(128 * tu, 128 * tv);
            array[id * 4 + (1 + tile.orientation) % 4].texCoords = sf::Vector2f(128 * (tu + 1), 128 * tv);
            array[id * 4 + (2 + tile.orientation) % 4].texCoords = sf::Vector2f(128 * (tu + 1), 128 * (tv + 1));
            array[id * 4 + (3 + tile.orientation) % 4].texCoords = sf::Vector2f(128 * tu, 128 * (tv + 1));
        }

        GameState::GameState(StateStack &stack, Context context):
            State(stack, context),
            m_lobby_done(false),
            m_desktop(),
            m_commands(5),
            m_server(nullptr),
            m_channel(&m_lobby_done),
            m_state(CurrentState::InLobby),
            m_selecting(false),
            m_selected_sprite(context.texture_holder->get("select_arrow"))
        {
            m_lobby = new game::Lobby(&m_lobby_done);
            m_lobby->add_to_desktop(m_desktop);

            m_view = get_context().window->getView();
            m_selected_sprite.setOrigin(12, 71);
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
            sf::VertexArray array(sf::Quads, m_size * m_size * 4);
            for(int i = 0; i < m_size * m_size; ++i) {
                add_to_vertex_array(array, m_tiles[i], i);
            }

            get_context().window->draw(array, &get_context().texture_holder->get("tiles"));

            for(auto &minion : m_minions) {
                if(minion.alive()) {
                    get_context().window->draw(minion);
                    if(minion.selected()) {
                        m_selected_sprite.setPosition(minion.get_x(), minion.get_y());
                        get_context().window->draw(m_selected_sprite);
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
        }

        bool GameState::update(sf::Time dt)
        {
            m_desktop.Update(dt.asSeconds());
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

            m_desktop.HandleEvent(event);

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
                } else if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left) {
                    m_select_end = m_select_start = get_context().window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));                   
                    m_selecting = true;
                } else if(m_selecting && event.type == sf::Event::MouseMoved) {
                    m_select_end = get_context().window->mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                } else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left) {
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
                m_desktop.Add(w);
            }
        }

        void GameState::waiting_update(sf::Time)
        {
            if(m_lobby_done) {
                delete m_server;
                m_server = nullptr;
                m_state = CurrentState::SettingUp;
                m_desktop.RemoveAll();
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
                if(tileid > 4) tileid = 4;
                m_tiles[i] = game::Tile(tileid, (game::Tile::Orientation)orientation_dist(m_random));
            }

            m_state = CurrentState::Playing;

            std::uniform_int_distribution<sf::Uint16> pos_dist(0, m_size * 128);

            std::uniform_real_distribution<float> colour_dist(0, 1);
            float h = colour_dist(m_random);
            m_player_colours.reserve(m_info.max_players);
            for(int i = 0; i < m_info.max_players; ++i) {
                m_player_colours[i] = get_colour(h);
                h += 0.618033;
                h -= (int)h;
            }

            std::uniform_int_distribution<sf::Uint8> player_dist(0, m_info.max_players - 1);
            for(int i = 0; i < 500; ++i) 
                create_minion(pos_dist(m_random), pos_dist(m_random), player_dist(m_random));
        }

        void GameState::playing_update(sf::Time dt)
        {
            update_input(dt);

            for(auto &minion : m_minions) {
                if(minion.alive())
                    minion.update(dt);
            }

            sf::Vector2f size = m_view.getSize();
            sf::Vector2f center = m_view.getCenter();

            if(center.x - size.x / 2.0 < 0)
                center.x = size.x / 2.0;
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
            std::uniform_int_distribution<sf::Uint8> hat_dist(0, 16);
            sf::Uint8 hatid = hat_dist(m_random);
            if(hatid >= 9)
                hatid = game::Minion::NO_HAT;
            game::Minion minion(hatid, m_player_colours[player_num], x, y, &get_context().texture_holder->get("minion"), &get_context().texture_holder->get("hats"), &get_context().shader_holder->get("minion"));

            sf::Uint16 minionid;

            if(!m_free_list.empty()) {
                auto itr = m_free_list.begin();
                m_minions[*itr] = minion;
                minionid = *itr;
                m_free_list.erase(itr);
            } else {
                minionid = m_minions.size();
                m_minions.push_back(minion);
            }

            if(player_num == m_my_player)
                m_my_minions.push_back(minionid);
        }

        void GameState::kill_minion(sf::Uint16 id)
        {
            m_minions[id].kill();
            m_free_list.insert(id);
        }
    }
}
