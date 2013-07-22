#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

namespace rts
{
    namespace gui
    {
        class Element : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
        {
            public:
                typedef std::unique_ptr<Element> ptr;

            public:
                explicit Element(Element *parent = nullptr);
                virtual ~Element();

                bool is_enabled() const;
                void enable();
                void disable();
                void toggle();

                bool is_selected() const;
                void select();
                void deselect();
                void toggle_selection();

                bool is_in_bounds(sf::Vector2<float> point);
                sf::Transform get_total_transform() const;

                virtual sf::FloatRect get_bounds() const = 0;
                virtual void handle_event(const sf::Event &event);

            private:
                Element *m_parent;

                bool m_enabled = true;
                bool m_selected = false;
        };
    }
}

#endif
