#include "state.hpp"

#include "rts/state_stack.hpp"

namespace rts
{
    namespace states
    {
        State::Context::Context(sf::RenderWindow &window):
            window(&window)
        {
        }

        State::State(StateStack &stack, Context context):
            m_stack(&stack), 
            m_context(context)
        {
        }

        State::~State() {}

        void State::request_stack_push(ID stateID)
        {
            m_stack->push_state(stateID);
        }

        void State::request_stack_pop()
        {
            m_stack->pop_state(); 
        }

        void State::request_stack_clear()
        {
            m_stack->clear_states();
        }

        State::Context State::get_context() const
        {
            return m_context; 
        }
    }
}
