#include "container.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace rts
{
    namespace gui
    {
        Container::Container(Element *parent):
            Element(parent)
        {
        }

        void Container::insert_element(Element *element)
        {
            m_elements.emplace_back(element);

            width = std::max(width, element->getTransform().transformRect(element->get_bounds()).width);
            height = std::max(height, element->getTransform().transformRect(element->get_bounds()).height);
        }

        sf::FloatRect Container::get_bounds() const
        {
            return sf::FloatRect(0.0, 0.0, width, height);
        }

        void Container::handle_event(const sf::Event &event)
        {
            for(auto element = m_elements.begin(); element != m_elements.end(); ++element)
                if((*element)->is_enabled())
                    (*element)->handle_event(event);
        }

        void Container::draw(sf::RenderTarget &target, sf::RenderStates states) const
        {
            states.transform *= getTransform();

            for(auto element = m_elements.begin(); element != m_elements.end(); ++element)
                target.draw(**element, states);
        }
    }
}
