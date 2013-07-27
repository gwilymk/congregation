#include "tile.hpp"
#include <set>

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

    sf::Uint8 connections[num_tiles][3] = {
        {0, 1, 2},
        {0, 1, 2},
        {0, 1, 2},
        {0, 1, 2},
        {0, 1, 2},
        {0, 1, 2},
        {1, 3, 2},
        {1, 2, 3},

        {3, 1, 2},
        {0, 3, 2},
        {0, 3, 2},
        {1, 2, 3},
        {2, 3, 2},
        {0, 3, 2},
        {1, 3, 2},
        {1, 1, 2},

        {3, 2, 2},
        {0, 1, 2},
        {0, 2, 2},
        {0, 1, 2},
        {0, 3, 2},
        {0, 1, 2},
        {0, 2, 2},
        {0, 1, 3},

        {0, 3, 2},
        {0, 2, 3},
        {0, 1, 2},
        {0, 1, 2},
        {1, 1, 2},
        {0, 3, 2},
        {0, 1, 2},
        {0, 1, 2},
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

            std::vector<Tile::Orientation> Tile::connected_to(Orientation direction)
            {
                sf::Uint8 abs_orientation = (direction + 4 - orientation) % 4;
                std::set<sf::Uint8> tmp;
                std::vector<Orientation> ret;
                sf::Uint8 *c = connections[id];
                sf::Uint8 current = abs_orientation;
                while(current < 3 && c[current] != current)
                    tmp.insert(current);

                for(sf::Uint8 i = current; i < 255; --i) {
                    for(sf::Uint8 already_in : tmp) {
                        if(c[i] == already_in)
                            tmp.insert(i);
                    }
                }

                for(sf::Uint8 dir : tmp) {
                    ret.push_back((Orientation) ((dir + orientation) % 4));
                }

                return ret;
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
