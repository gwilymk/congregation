#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Config.hpp>
#include <SFML/Network/Packet.hpp>

namespace rts
{
    namespace states
    {
        namespace game
        {
            struct Tile
            {
                sf::Uint8 id;

                enum Orientation : sf::Uint8 { NORTH, EAST, SOUTH, WEST };
                Orientation orientation;
            };

            sf::Packet &operator<<(sf::Packet &packet, const Tile &tile);
            sf::Packet &operator>>(sf::Packet &packet, Tile &tile);
        }
    }
}

#endif
