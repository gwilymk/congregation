#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "rts/state_stack.hpp"
#include "rts/holders/holder.hpp"
#include "rts/holders/music_holder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFGUI/SFGUI.hpp>

namespace rts
{
    class Application : private sf::NonCopyable
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
            sfg::Desktop m_desktop;
            sf::RenderWindow m_render_window;
            holders::TextureHolder m_texture_holder;
            holders::FontHolder m_font_holder;
            holders::ShaderHolder m_shader_holder;
            holders::SoundHolder m_sound_holder;
            holders::MusicHolder m_music_holder;
            bool m_focused;
            StateStack m_state_stack;
    };
}

#endif
