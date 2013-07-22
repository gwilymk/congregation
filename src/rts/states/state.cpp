#include "state.hpp"

namespace rts
{
    namespace states
    {
        State::Context::Context(sf::RenderWindow &window):
            window(&window)
        {
        }
    }
}
