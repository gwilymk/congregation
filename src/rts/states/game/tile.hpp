/*
    Congregation - An rts game involving the placement of tiles
    Copyright (C) 2013  Gwilym Kuiper (kuiper.gwilym@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
                sf::Uint32 check_time = -1;
                bool cache = false;

                bool has_watchtower();
                EdgeFeature get_feature(Orientation direction) const;

                Orientation get_direction(sf::Uint16 x, sf::Uint16 y) const;

                std::vector<Orientation> connected_to(Orientation direction);

                static sf::Uint16 get_total_probability();
                static sf::Uint8 get_tile(sf::Uint16 prob);
            };

            sf::Packet &operator<<(sf::Packet &packet, const Tile &tile);
            sf::Packet &operator>>(sf::Packet &packet, Tile &tile);
        }
    }
}

#endif
