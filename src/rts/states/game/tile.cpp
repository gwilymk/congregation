#include "tile.hpp"

namespace rts
{
    namespace states
    {
        namespace game
        {
            sf::Packet &operator<<(sf::Packet &packet, const Tile &tile)
            {
                packet << tile.id;
                return packet << (sf::Uint8 &) tile.orientation;
            }

            sf::Packet &operator>>(sf::Packet &packet, Tile &tile)
            {
                packet >> tile.id;
                return packet >> (sf::Uint8 &) tile.orientation;
            }
        }
    }
}
