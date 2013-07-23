#include "game_state.hpp"

namespace rts
{
    namespace states
    {
        GameState::GameState(StateStack &stack, Context context):
            State(stack, context),
            m_lobby_done(false),
            m_desktop(),
            m_commands(5),
            m_lobby(&m_lobby_done)
        {
            m_lobby.add_to_desktop(m_desktop);
        }

        void GameState::draw()
        {
        }

        bool GameState::update(sf::Time dt)
        {
            m_desktop.Update(dt.asSeconds());
            if(!m_lobby_done)
                m_lobby.update();

            return true;
        }

        bool GameState::handle_event(const sf::Event &event)
        {
            m_desktop.HandleEvent(event);
            return true;
        }
    }
}
