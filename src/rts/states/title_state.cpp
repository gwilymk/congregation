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
            get_context().music_holder->play("theme");
            m_text.setPosition(x, y);
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
                    request_stack_push(ID::MainMenuState);
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
