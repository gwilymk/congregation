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
