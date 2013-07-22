#include "main_menu.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "rts/gui/button.hpp"

namespace rts
{
    namespace states
    {
        MainMenuState::MainMenuState(StateStack &stack, Context context):
            State(stack, context),
            m_container(nullptr)
        {
            m_container.insert_element(new gui::Button(&m_container, "Play Singleplayer", get_context().font_holder->get("font"), get_context().texture_holder->get("title"),
            [this] () {

            })); 

            gui::Button *b2 = new gui::Button(&m_container, "Play Multiplayer", get_context().font_holder->get("font"), get_context().texture_holder->get("title"),
            [this] () {
            
            });
            b2->move(0, b2->get_bounds().height);

            m_container.insert_element(b2);

            m_container.setPosition(100, 300);
        }

        void MainMenuState::draw()
        {
            get_context().window->draw(m_container);
        }

        bool MainMenuState::update(sf::Time dt)
        {
            return true;
        }

        bool MainMenuState::handle_event(const sf::Event &event)
        {
            switch(event.type) {
                case sf::Event::Closed:
                    request_stack_pop();
                    break;
                default:
                    m_container.handle_event(event);
                    break;
            }

            return true;
        }
    }
}
