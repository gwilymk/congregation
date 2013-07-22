#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "rts/states/state.hpp"

namespace rts
{
    namespace states
    {
        class MainMenuState : public State
        {
            public:
                MainMenuState(StateStack &states, Context context);

                virtual void draw();
                virtual bool update(sf::Time dt);
                virtual bool handle_event(const sf::Event &event);
        };
    }
}

#endif
