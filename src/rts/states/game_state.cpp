#include "game_state.hpp"
#include "rts/states/game/lobby.hpp"
#include "rts/network/server.hpp"

#include <SFGUI/SFGUI.hpp>

namespace rts
{
    namespace states
    {
        GameState::GameState(StateStack &stack, Context context):
            State(stack, context),
            m_lobby_done(false),
            m_desktop(),
            m_commands(5),
            m_server(nullptr),
            m_channel(),
            m_state(CurrentState::InLobby)
        {
            m_lobby = new game::Lobby(&m_lobby_done);
            m_lobby->add_to_desktop(m_desktop);
        }

        GameState::~GameState()
        {
            delete m_lobby;
            delete m_server;
        }

        void GameState::draw()
        {
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
                case CurrentState::Playing:
                    break;
            }

            return true;
        }

        bool GameState::handle_event(const sf::Event &event)
        {
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
                    m_channel.connect_to_server(sf::IpAddress::LocalHost, m_server->port());
                } else
                    m_channel.connect_to_server(m_lobby->get_server(), m_lobby->get_server_port());


                delete m_lobby;
                m_lobby = nullptr;

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

        void GameState::waiting_update(sf::Time dt)
        {
            if(m_server)
                m_server->listen();
            m_channel.update();

            std::ostringstream ss;
            ss << "Waiting for other players (" << m_channel.num_players() << '/' << m_channel.max_players() << ')';
            m_status_label->SetText(ss.str());
        }
    }
}
