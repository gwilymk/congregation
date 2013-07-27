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
            const int num_grass_tiles = 5;
            const int num_tiles = 32;

            struct Tile
            {
                enum Orientation : sf::Uint8 { NORTH, EAST, SOUTH, WEST };
                Tile();
                Tile(sf::Uint8 id, Orientation orientation);

                Orientation orientation;
                sf::Uint8 id;
            };

            sf::Packet &operator<<(sf::Packet &packet, const Tile &tile);
            sf::Packet &operator>>(sf::Packet &packet, Tile &tile);
        }
    }
}

#endif
