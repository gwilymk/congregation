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

#ifndef STATE_HPP
#define STATE_HPP

#include <memory>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include "id.hpp"
#include "rts/holders/holder.hpp"

namespace sf
{
    class Event;
    class RenderWindow;
}

namespace sfg { class Desktop; }

namespace rts
{
    class StateStack;
    struct Settings;

    namespace holders { class MusicHolder; }

    namespace states
    {
        class State : private sf::NonCopyable
        {
            public:
                typedef std::unique_ptr<State> ptr;

                struct Context {
                    Context(sf::RenderWindow &window, holders::TextureHolder &texture_holder, holders::FontHolder &font_holder, holders::ShaderHolder &shader_holder, holders::SoundHolder &sound_holder, holders::MusicHolder &music_holder, sfg::Desktop &desktop, bool &focused, Settings &settings);

                    sf::RenderWindow *window;
                    holders::TextureHolder *texture_holder;
                    holders::FontHolder *font_holder;
                    holders::ShaderHolder *shader_holder;
                    holders::SoundHolder *sound_holder;
                    holders::MusicHolder *music_holder;
                    sfg::Desktop *desktop;
                    bool *focused;
                    Settings *settings;
                };

            public:
                State(StateStack &stack, Context context);
                virtual ~State();
                
                virtual void draw() = 0;
                virtual bool update(sf::Time dt) = 0;
                virtual bool handle_event(const sf::Event &event) = 0;

            protected:
                void request_stack_push(ID stateID);
                void request_stack_pop();
                void request_stack_clear();

                Context get_context() const;

            private:
                StateStack *m_stack;
                Context m_context;
        };
    }
}

#endif
