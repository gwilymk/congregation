#include "main_menu.hpp"

namespace rts
{
    namespace states
    {
        MainMenuState::MainMenuState(StateStack &stack, Context context):
            State(stack, context)
        {
        }

        void MainMenuState::draw()
        {
        }

        bool MainMenuState::update(sf::Time dt)
        {
            return true;
        }

        bool MainMenuState::handle_event(const sf::Event &event)
        {
            return true;
        }
    }
}
