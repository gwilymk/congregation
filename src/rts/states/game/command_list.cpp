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
