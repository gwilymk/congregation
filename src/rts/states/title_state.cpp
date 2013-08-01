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

#include "title_state.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include "rts/holders/music_holder.hpp"

#include <cmath>

namespace rts
{
    namespace states
    {
        TitleState::TitleState(StateStack &stack, Context context):
            State(stack, context),
            m_background_sprite(context.texture_holder->get("title")),
            m_text("Press Any Key To Continue", context.font_holder->get("font")),
            m_text_effect_time(sf::Time::Zero)
        {
            float x = (get_context().window->getSize().x - m_text.getLocalBounds().width) / 2.0;
            float y = (get_context().window->getSize().y - m_text.getLocalBounds().height) / 2.0;
            //get_context().music_holder->play("theme");
            m_text.setPosition(x, y + 200);
        }

        void TitleState::draw()
        {
            get_context().window->draw(m_background_sprite);
            m_text.setColor(sf::Color(255, 255, 255, 255.0 * (std::sin(m_text_effect_time.asSeconds() * 2.0) + 1.0) / 2.0));
            get_context().window->draw(m_text);
        }

        bool TitleState::update(sf::Time dt)
        {
            m_text_effect_time += dt;
            return true;
        }

        bool TitleState::handle_event(const sf::Event &event)
        {
            switch(event.type) {
                case sf::Event::MouseButtonPressed:
                case sf::Event::KeyPressed:
                    request_stack_pop();
                    request_stack_push(ID::GameState);
                    break;
                case sf::Event::Closed:
                    request_stack_pop();
                default:
                    break;
            }

            return true;
        }
    }
}
