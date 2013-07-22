#ifndef TITLE_STATE_HPP
#define TITLE_STATE_HPP

#include "rts/states/state.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace rts
{
    namespace states
    {
        class TitleState : public State
        {
            public:
                TitleState(StateStack &stack, Context context);

                virtual void draw();
                virtual bool update(sf::Time dt);
                virtual bool handle_event(const sf::Event &event);

            private:
                sf::Sprite m_background_sprite;
                sf::Text m_text;

                sf::Time m_text_effect_time;
        };
    }
}

#endif
