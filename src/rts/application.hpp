/*
    Congregation - An rts game involving the placement of tiles
    Copyright (C) 2013  Gwilym Kuiper (kuiper.gwilym@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
