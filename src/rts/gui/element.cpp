#include "element.hpp"

namespace rts
{
    namespace gui
    {
        Element::Element(Element *parent):
            m_parent(parent)
        {
        }

        Element::~Element()
        {
        }

        void Element::enable()
        {
            m_enabled = true;
        }

        void Element::disable()
        {
            m_enabled = false;
        }

        void Element::toggle()
        {
            m_enabled = !m_enabled;
        }

        bool Element::is_enabled() const
        {
            return m_enabled;
        }

        void Element::select()
        {
            m_selected = true;
        }

        void Element::deselect()
        {
            m_selected = false;
        }

        void Element::toggle_selection()
        {
            m_selected = !m_selected;
        }

        bool Element::is_selected() const
        {
            return m_selected;
        }

        bool Element::is_in_bounds(sf::Vector2<float> point)
        {
            return get_total_transform().transformRect(get_bounds()).contains(point);
        }

        sf::Transform Element::get_total_transform() const
        {
            if(m_parent)
                return getTransform() * m_parent->get_total_transform();
            else
                return getTransform();
        }

        void Element::handle_event(const sf::Event &event)
        {
        }
    }
}
