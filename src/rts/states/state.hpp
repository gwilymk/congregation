#ifndef STATE_HPP
#define STATE_HPP

#include <memory>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include "id.hpp"

namespace sf
{
    class Event;
    class RenderWindow;
}

namespace rts
{
    class StateStack;

    namespace states
    {
        class State : private sf::NonCopyable
        {
            public:
                typedef std::unique_ptr<State> ptr;

                struct Context {
                    Context(sf::RenderWindow &window);

                    sf::RenderWindow *window;
                };

            public:
                State(StateStack &stack, Context context);
                virtual ~State();
                
                virtual void draw() = 0;
                virtual bool update(sf::Time dt) = 0;
                virtual bool handle_event(const sf::Event &event) = 0;

            protected:
                void request_stack_push(ID stateID);
                void request_stack_pop();
                void request_stack_clear();

                Context get_context() const;

            private:
                StateStack *m_stack;
                Context m_context;
        };
    }
}

#endif
