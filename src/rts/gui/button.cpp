#include "button.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace rts
{
    namespace gui
    {
        Button::Button(Element *parent, const std::string &text, const sf::Font &font, const sf::Texture &texture, std::function<void()> callback):
            Element(parent),
            m_label(this, text, font),
            m_background(texture),
            m_callback(callback)
        {
            m_background.setScale((m_label.get_bounds().width + 10) / m_background.getLocalBounds().width, (m_label.get_bounds().height + 10) / m_background.getLocalBounds().height);
            m_label.setPosition(2.5, 2.5);
        }

        sf::FloatRect Button::get_bounds() const
        {
            return sf::FloatRect(0.0, 0.0, m_label.get_bounds().width + 10.0, m_label.get_bounds().height + 10.0);
        }

        void Button::handle_event(const sf::Event &event)
        {
            switch(event.type) {
                case sf::Event::MouseButtonPressed:
                    if(event.mouseButton.button == sf::Mouse::Button::Left) {
                        if(is_in_bounds(sf::Vector2<float>(event.mouseButton.x, event.mouseButton.y)))
                            select();
                        else
                            deselect();
                    }
                    break;

                case sf::Event::MouseButtonReleased:
                    if(event.mouseButton.button == sf::Mouse::Button::Left)
                        if(is_in_bounds(sf::Vector2<float>(event.mouseButton.x, event.mouseButton.y)))
                            m_callback();
                    break;

                default:
                    break;
            }
        }

        void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const
        {
            states.transform *= getTransform();
            target.draw(m_background, states);
            target.draw(m_label, states);
        }
    }
}
