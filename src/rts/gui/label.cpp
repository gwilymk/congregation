#include "label.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace rts
{
    namespace gui
    {
        Label::Label(Element *parent, const std::string &text, const sf::Font &font, unsigned int size):
            Element(parent),
            m_text(text, font, size)
        {
        }

        sf::FloatRect Label::get_bounds() const
        {
            return m_text.getLocalBounds();
        }

        void Label::set_colour(sf::Color colour)
        {
            m_text.setColor(colour);
        }

        void Label::draw(sf::RenderTarget &target, sf::RenderStates state) const
        {
            state.transform *= getTransform();
            target.draw(m_text, state);
        }
    }
}
