#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "rts/gui/element.hpp"
#include "rts/gui/label.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <functional>

namespace rts
{
    namespace gui
    {
        class Button : public Element
        {
            public:
                Button(Element *parent, const std::string &text, const sf::Font &font, const sf::Texture &texture, std::function<void()> callback);

                virtual void handle_event(const sf::Event &event);
                virtual sf::FloatRect get_bounds() const;

            protected:
                virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

            private:
                Label m_label;
                sf::Sprite m_background;
                std::function<void()> m_callback;
        };
    }
}

#endif
