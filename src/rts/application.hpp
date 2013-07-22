#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "rts/state_stack.hpp"
#include "rts/lua/state.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

namespace rts
{
    class Application
    {
        public:
            Application();

            int run();

        private:
            void process_input();
            void update(sf::Time dt);
            void draw();

            void register_states();

        private:
            sf::RenderWindow m_render_window;
            lua::State m_lua_state;
            StateStack m_state_stack;
    };
}

#endif
