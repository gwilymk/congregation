#include "main_menu.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

namespace rts
{
    namespace states
    {
        MainMenuState::MainMenuState(StateStack &stack, Context context):
            State(stack, context)
        {
            m_box = sfg::Box::Create(sfg::Box::VERTICAL, 1.5f);

            sfg::Button::Ptr b1 = sfg::Button::Create("Single Player");
            b1->GetSignal(sfg::Button::OnLeftClick).Connect(&MainMenuState::start_singleplayer, this);
            m_box->Pack(b1);
            sfg::Button::Ptr b2 = sfg::Button::Create("Multi Player");
            b2->GetSignal(sfg::Button::OnLeftClick).Connect(&MainMenuState::start_multiplayer, this);
            m_box->Pack(b2);

            m_box->SetPosition(sf::Vector2f(100, 300));
        }

        void MainMenuState::draw()
        {
        }

        bool MainMenuState::update(sf::Time dt)
        {
            m_box->Update(dt.asSeconds());
            return true;
        }

        bool MainMenuState::handle_event(const sf::Event &event)
        {
            switch(event.type) {
                case sf::Event::Closed:
                    request_stack_pop();
                    break;
                default:
                    m_box->HandleEvent(event);
                    break;
            }

            return true;
        }

        void MainMenuState::start_singleplayer()
        {
        }

        void MainMenuState::start_multiplayer()
        {
        }

        void MainMenuState::settings()
        {
        }
    }
}
