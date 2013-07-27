#include "tile.hpp"

namespace
{
    using namespace rts::states::game;

    Tile::EdgeFeature GRASS = Tile::EdgeFeature::GRASS;
    Tile::EdgeFeature CITY = Tile::EdgeFeature::CITY;
    Tile::EdgeFeature ROAD = Tile::EdgeFeature::ROAD;

    Tile::EdgeFeature tile_features[num_tiles][4] = {
        {GRASS, GRASS, GRASS, GRASS},
        {GRASS, GRASS, GRASS, GRASS},
        {GRASS, GRASS, GRASS, GRASS},
        {GRASS, GRASS, GRASS, GRASS},
        {GRASS, GRASS, GRASS, GRASS},
        {GRASS, CITY,  GRASS, GRASS},
        {ROAD,  ROAD,  GRASS, ROAD },
        {ROAD,  ROAD,  ROAD,  ROAD },
        {ROAD,  GRASS, GRASS, ROAD },
        {GRASS, ROAD,  GRASS, ROAD },
        {GRASS, ROAD,  GRASS, ROAD },
        {CITY,  CITY,  CITY,  CITY },
        {ROAD,  CITY,  ROAD,  CITY },
        {GRASS, CITY,  GRASS, CITY },
        {CITY,  CITY,  GRASS, CITY },
        {CITY,  CITY,  GRASS, GRASS},
        {CITY,  ROAD,  ROAD,  CITY },
        {CITY,  GRASS, GRASS, CITY },
        {CITY,  ROAD,  ROAD,  CITY },
        {CITY,  GRASS, CITY,  GRASS},
        {CITY,  ROAD,  CITY,  ROAD },
        {CITY,  GRASS, GRASS, GRASS},
        {CITY,  ROAD,  ROAD,  GRASS},
        {CITY,  GRASS, ROAD,  ROAD },
        {CITY,  ROAD,  GRASS, ROAD },
        {CITY,  ROAD,  ROAD,  ROAD },
        {GRASS, ROAD,  GRASS, CITY },
        {CITY,  GRASS, CITY,  CITY },
        {ROAD,  ROAD,  GRASS, GRASS},
        {GRASS, ROAD,  GRASS, ROAD },
        {GRASS, GRASS, ROAD,  GRASS},
        {GRASS, GRASS, GRASS, GRASS},
    };
}

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

            Tile::EdgeFeature Tile::get_feature(Orientation direction)
            {
                return tile_features[id][(int)(direction + 4 - orientation) % 4];
            }

            bool Tile::has_watchtower()
            {
                return id == 30 || id == 31;
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
