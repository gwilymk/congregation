#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "rts/states/state.hpp"
#include "rts/states/game/command_list.hpp"

#include <SFGUI/Desktop.hpp>

namespace rts
{
    namespace states
    {
        class GameState : public State
        {
            public:
                GameState(StateStack &stack, Context context);

                virtual void draw();
                virtual bool update(sf::Time dt);
                virtual bool handle_event(const sf::Event &event);

            private:
                sfg::Desktop m_desktop;
                game::CommandList m_commands;
        };
    }
}

#endif
