#include "game_state.hpp"

namespace rts
{
    namespace states
    {
        GameState::GameState(StateStack &stack, Context context):
            State(stack, context)
        {
        }

        void GameState::draw()
        {
        }

        bool GameState::update(sf::Time dt)
        {
            return true;
        }

        bool GameState::handle_event(const sf::Event &event)
        {
            return true;
        }
    }
}
