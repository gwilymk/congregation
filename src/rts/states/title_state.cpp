#include "title_state.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

namespace rts
{
    namespace states
    {
        TitleState::TitleState(StateStack &stack, Context context):
            State(stack, context),
            m_background_sprite(context.texture_holder->get("title"))
        {
        }

        void TitleState::draw()
        {
            get_context().window->draw(m_background_sprite);
        }

        bool TitleState::update(sf::Time dt)
        {
            return true;
        }

        bool TitleState::handle_event(const sf::Event &event)
        {
            return true;
        }
    }
}
