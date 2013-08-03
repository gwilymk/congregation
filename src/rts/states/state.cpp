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

#include "state.hpp"

#include "rts/state_stack.hpp"

namespace rts
{
    namespace states
    {
        State::Context::Context(sf::RenderWindow &window, holders::TextureHolder &texture_holder, holders::FontHolder &font_holder, holders::ShaderHolder &shader_holder, holders::SoundHolder &sound_holder, holders::MusicHolder &music_holder, sfg::Desktop &desktop, bool &focused, Settings &settings):
            window(&window),
            texture_holder(&texture_holder),
            font_holder(&font_holder),
            shader_holder(&shader_holder),
            sound_holder(&sound_holder),
            music_holder(&music_holder),
            desktop(&desktop),
            focused(&focused),
            settings(&settings)
        {
        }

        State::State(StateStack &stack, Context context):
            m_stack(&stack), 
            m_context(context)
        {
        }

        State::~State() {}

        void State::request_stack_push(ID stateID)
        {
            m_stack->push_state(stateID);
        }

        void State::request_stack_pop()
        {
            m_stack->pop_state(); 
        }

        void State::request_stack_clear()
        {
            m_stack->clear_states();
        }

        State::Context State::get_context() const
        {
            return m_context; 
        }
    }
}
