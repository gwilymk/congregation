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
            class CommandList : private sf::NonCopyable
            {
                public:
                    explicit CommandList(int turns_to_store);

                    void add_command(const Command &command);
                    void next_turn();

                    Command get_command();

                private:
                    std::set<Command, bool(*)(const Command &a, const Command &b)> m_commands;
                    int m_turns_to_store;
                    unsigned int m_current_turn;
            };
        }
    }
}

#endif
