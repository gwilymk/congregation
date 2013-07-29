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

#include "command_list.hpp"
#include "rts/common.hpp"

namespace rts
{
    namespace states
    {
        namespace game
        {
            CommandList::CommandList():
                m_commands(&Command::less),
                m_current_turn(0)
            {
            }

            void CommandList::add_command(const Command &command)
            {
                Command tmp = command;
                tmp.turn = m_current_turn + num_turns_to_store;
                m_commands.insert(command);
            }

            void CommandList::next_turn()
            {
                ++m_current_turn;
            }

            sf::Uint32 CommandList::get_turn() const
            {
                return m_current_turn;
            }

            Command CommandList::get_command()
            {
                if(m_commands.empty())
                    return Command();

                auto itr = m_commands.begin();
                ASSERT(itr->turn >= m_current_turn);

                if(itr->turn != m_current_turn)
                    return Command();

                Command ret = *itr;
                m_commands.erase(itr);
                return ret;
            }
        }
    }
}
