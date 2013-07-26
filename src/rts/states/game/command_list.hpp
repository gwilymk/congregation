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
