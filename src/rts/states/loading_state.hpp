#ifndef LOADING_STATE_HPP
#define LOADING_STATE_HPP

#include "rts/states/state.hpp"
#include "rts/lua/state.hpp"

namespace rts
{
    namespace states
    {
        class LoadingState : public State
        {
            public:
                LoadingState(StateStack &stack, Context context);

                virtual void draw();
                virtual bool update(sf::Time dt);
                virtual bool handle_event(const sf::Event &event);

            private:
                lua::State m_lua_state;
        };
    }
}

#endif
