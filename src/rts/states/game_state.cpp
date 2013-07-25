#include "game_state.hpp"
#include "rts/states/game/lobby.hpp"
#include "rts/network/server.hpp"

#include <SFGUI/SFGUI.hpp>

namespace
{
    const float move_speed = 5.0;
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
            m_state(CurrentState::InLobby)
        {
            m_lobby = new game::Lobby(&m_lobby_done);
            m_lobby->add_to_desktop(m_desktop);

            m_view = get_context().window->getView();
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
            std::array<sf::Uint32, network::seed_size> seed = m_channel.get_seed();
            std::seed_seq seq(seed.begin(), seed.end());
            m_random.seed(seq);

            sf::Uint8 map_size = m_info.map_size;
            m_size = network::map_sizes[map_size];

            m_tiles.reserve(m_size * m_size);
            std::geometric_distribution<sf::Uint8> grass_dist(0.75);
            std::uniform_int_distribution<sf::Uint8> orientation_dist(game::Tile::Orientation::NORTH, game::Tile::Orientation::WEST);

            for(int i = 0 ; i < m_size * m_size ; ++i) {
                sf::Uint8 tileid = grass_dist(m_random);
                if(tileid > 4) tileid = 4;
                m_tiles[i] = game::Tile(tileid, (game::Tile::Orientation)orientation_dist(m_random));
            }

            m_state = CurrentState::Playing;
        }

        void GameState::playing_update(sf::Time dt)
        {
            update_input(dt);

            get_context().window->setView(m_view);
        }

        void GameState::update_input(sf::Time dt)
        {
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::K))
                m_view.move(0, -dt.asSeconds() * 128.0 * move_speed);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::H))
                m_view.move(-dt.asSeconds() * 128.0 * move_speed, 0);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::J))
                m_view.move(0, dt.asSeconds() * 128.0 * move_speed);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::L))
                m_view.move(dt.asSeconds() * 128.0 * move_speed, 0);
        }
    }
}
