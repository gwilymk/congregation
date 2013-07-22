#include "state.hpp"

namespace rts
{
    namespace states
    {
        State::Context::Context(sf::RenderWindow &window, lua::State &L):
            window(&window),
            L(&L)
        {
        }
    }
}
