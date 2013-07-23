#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "rts/states/state.hpp"

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
        };
    }
}

#endif
