#ifndef STATE_HPP
#define STATE_HPP

#include <memory>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include "id.hpp"
#include "rts/holders/holder.hpp"

namespace sf
{
    class Event;
    class RenderWindow;
}

namespace sfg { class Desktop; }

namespace rts
{
    class StateStack;

    namespace holders { class MusicHolder; }

    namespace states
    {
        class State : private sf::NonCopyable
        {
            public:
                typedef std::unique_ptr<State> ptr;

                struct Context {
                    Context(sf::RenderWindow &window, holders::TextureHolder &texture_holder, holders::FontHolder &font_holder, holders::ShaderHolder &shader_holder, holders::SoundHolder &sound_holder, holders::MusicHolder &music_holder, sfg::Desktop &desktop, bool &focused);

                    sf::RenderWindow *window;
                    holders::TextureHolder *texture_holder;
                    holders::FontHolder *font_holder;
                    holders::ShaderHolder *shader_holder;
                    holders::SoundHolder *sound_holder;
                    holders::MusicHolder *music_holder;
                    sfg::Desktop *desktop;
                    bool *focused;
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
