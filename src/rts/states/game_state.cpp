#include "game_state.hpp"
#include "rts/states/game/lobby.hpp"

namespace rts
{
    namespace states
    {
        GameState::GameState(StateStack &stack, Context context):
            State(stack, context),
            m_lobby_done(false),
            m_desktop(),
            m_commands(5),
            m_state(CurrentState::InLobby)
        {
            m_lobby = new game::Lobby(&m_lobby_done);
            m_lobby->add_to_desktop(m_desktop);
        }

        GameState::~GameState()
        {
            delete m_lobby;
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
                
                } else {
                
                }

                delete m_lobby;
                m_lobby = nullptr;

                m_state = CurrentState::Waiting;
            }
        }
    }
}
