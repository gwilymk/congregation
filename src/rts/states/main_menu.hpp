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

#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "rts/states/state.hpp"
#include <SFGUI/Box.hpp>
#include <SFGUI/Button.hpp>

namespace rts
{
    namespace states
    {
        class MainMenuState : public State
        {
            public:
                MainMenuState(StateStack &states, Context context);

                virtual void draw();
                virtual bool update(sf::Time dt);
                virtual bool handle_event(const sf::Event &event);

            private:
                void start_singleplayer();
                void start_multiplayer();
                void settings();

            private:
                sfg::Box::Ptr m_box;
        };
    }
}

#endif
