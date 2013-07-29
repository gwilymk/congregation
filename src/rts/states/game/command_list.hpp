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

#ifndef COMMAND_LIST_HPP
#define COMMAND_LIST_HPP

#include <rts/states/game/command.hpp>

#include <SFML/System/NonCopyable.hpp>
#include <set>

namespace rts
{
    namespace states
    {
        namespace game
        {
            const int num_turns_to_store = 5;

            class CommandList : private sf::NonCopyable
            {
                public:
                    CommandList();

                    void add_command(const Command &command);
                    void next_turn();

                    sf::Uint32 get_turn() const;
                    Command get_command();

                private:
                    std::set<Command, bool(*)(const Command &a, const Command &b)> m_commands;
                    sf::Uint32 m_current_turn;
            };
        }
    }
}

#endif
