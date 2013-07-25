#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "rts/state_stack.hpp"
#include "rts/holders/holder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFGUI/SFGUI.hpp>

namespace rts
{
    class Application
    {
        public:
            Application();

            int run();

        private:
            void init();

            void process_input();
            void update(sf::Time dt);
            void draw();

            void register_states();

        private:
            sfg::SFGUI m_sfgui;
            sf::RenderWindow m_render_window;
            holders::TextureHolder m_texture_holder;
            holders::FontHolder m_font_holder;
            holders::ShaderHolder m_shader_holder;
            bool m_focused;
            StateStack m_state_stack;
    };
}

#endif
