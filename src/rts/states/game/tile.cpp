#include "tile.hpp"

namespace rts
{
    namespace states
    {
        namespace game
        {
            Tile::Tile():
                orientation(orientation),
                id(0)
            {
            }

            Tile::Tile(sf::Uint8 id, Orientation orientation):
                orientation(orientation),
                id(id)
            {
            }

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
