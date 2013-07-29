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

#include "rts/states/game/tile.hpp"
#include <iostream>

using namespace rts::states::game;
using namespace std;

void print_tile_connected_to(sf::Uint8 id, Tile::Orientation rotate, Tile::Orientation direction)
{
    Tile tile;
    tile.id = id;
    tile.orientation = rotate;
    std::vector<Tile::Orientation> con_to = tile.connected_to(direction);

    std::cout << "Tile " << (unsigned)id + 1 << " when rotated by " << (unsigned) rotate << " has";
    
    if(con_to.empty()) {
        std::cout << " no connections";
    } else {
        std::cout << " connections ";
    }

    for(auto i : con_to) {
        std::cout << (unsigned) i << " ";
    }
    std::cout << " to feature " << (unsigned) tile.get_feature(direction) << " on its " << (unsigned) direction << " side\n";
}

int main(int, char **)
{
    for(int i = 0; i < num_tiles; ++i) {
        for(int j = 0; j < 4; ++j) {
            for(int k = 0; k < 4; ++k) {
                print_tile_connected_to(i, (Tile::Orientation)j, (Tile::Orientation)k);
            }
        }
    }
}
