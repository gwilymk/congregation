#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "rts/gui/element.hpp"
#include <vector>

namespace rts
{
    namespace gui
    {
        class Container : public Element
        {
            public:
                Container(Element *parent);

                void insert_element(Element *element);
                
                virtual void handle_event(const sf::Event &event);
                virtual sf::FloatRect get_bounds() const;

            protected:
                virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

            private:
                std::vector<Element::ptr> m_elements;
                float width = 0.0;
                float height = 0.0;
        };
    }
}

#endif
