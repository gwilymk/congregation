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
                enum EdgeFeature { GRASS, ROAD, CITY };

                Tile();
                Tile(sf::Uint8 id, Orientation orientation);

                Orientation orientation;
                sf::Uint8 id;

                bool has_watchtower();
                EdgeFeature get_feature(Orientation direction);

                std::vector<Orientation> connected_to(Orientation direction);
            };

            sf::Packet &operator<<(sf::Packet &packet, const Tile &tile);
            sf::Packet &operator>>(sf::Packet &packet, Tile &tile);
        }
    }
}

#endif
