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
