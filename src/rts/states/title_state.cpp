#include "title_state.hpp"

namespace rts
{
    namespace states
    {
        TitleState::TitleState(StateStack &stack, Context context):
            State(stack, context)
        {
        }

        void TitleState::draw()
        {
        }

        bool TitleState::update(sf::Time dt)
        {
            return true;
        }

        bool TitleState::handle_event(const sf::Event &event)
        {
            return true;
        }
    }
}
