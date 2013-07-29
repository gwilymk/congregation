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

#ifndef STATE_STACK_HPP
#define STATE_STACK_HPP

#include <SFML/System/NonCopyable.hpp>
#include <vector>
#include <map>

#include "rts/states/state.hpp"

namespace rts
{
    class StateStack : private sf::NonCopyable
    {
        public:
            enum class Action
            {
                Push,
                Pop,
                Clear,
            };

        public:
            explicit StateStack(states::State::Context context);
            ~StateStack();

            template <class T> void register_state(states::ID stateID);

            void update(sf::Time dt);
            void draw();
            void handle_event(const sf::Event &event);

            void push_state(states::ID stateID);
            void pop_state();
            void clear_states();

            bool is_empty();

        private:
            states::State::ptr create_state(states::ID stateID);
            void apply_pending_changes();

        private:
            struct PendingChange
            {
                PendingChange(Action action, states::ID stateID = states::ID::NumStates);

                Action action;
                states::ID stateID;
            };

        private:
            std::vector<states::State::ptr> m_stack;
            std::vector<PendingChange> m_pending_list;
            states::State::Context m_context;
            std::map<states::ID, std::function<states::State::ptr()>> m_factories;
    };
}

#include "state_stack.inl"

#endif
