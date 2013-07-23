#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "rts/states/state.hpp"
#include <SFGUI/Box.hpp>
#include <SFGUI/Button.hpp>

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

            private:
                void start_singleplayer();
                void start_multiplayer();
                void settings();

            private:
                sfg::Box::Ptr m_box;
        };
    }
}

#endif
