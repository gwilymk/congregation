#ifndef LABEL_HPP
#define LABEL_HPP

#include "rts/gui/element.hpp"
#include <SFML/Graphics/Text.hpp>

namespace rts
{
    namespace gui
    {
        class Label : public Element
        {
            public:
                Label(Element *parent, const std::string &text, const sf::Font &font, unsigned int size = 30);

                virtual sf::FloatRect get_bounds() const;
                void set_colour(sf::Color colour);

            protected:
                virtual void draw(sf::RenderTarget &target, sf::RenderStates state) const;

            private:
                sf::Text m_text;
        };
    }
}

#endif
