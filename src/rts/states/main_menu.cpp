/*
    Congregation - An rts game involving the placement of tiles
    Copyright (C) 2013  Gwilym Kuiper (kuiper.gwilym@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

            sfg::Button::Ptr b2 = sfg::Button::Create("Play");
            b2->GetSignal(sfg::Button::OnLeftClick).Connect(&MainMenuState::start_multiplayer, this);
            m_box->Pack(b2);
            sfg::Button::Ptr b4 = sfg::Button::Create("Quit");
            b4->GetSignal(sfg::Button::OnLeftClick).Connect(&MainMenuState::request_stack_pop, (State *)this);
            m_box->Pack(b4);

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

        void MainMenuState::start_multiplayer()
        {
            request_stack_pop();
            request_stack_push(ID::GameState);
        }

        void MainMenuState::settings()
        {
        }
    }
}
