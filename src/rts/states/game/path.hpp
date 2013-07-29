#ifndef PATH_HPP
#define PATH_HPP

#include <vector>
#include <deque>
#include <SFML/Config.hpp>
#include "rts/states/game/tile.hpp"

namespace rts
{
    namespace states
    {
        namespace game
        {
            class Path
            {
                public:
                    explicit Path(sf::Uint16 map_size);
                    Path(sf::Uint16 startx, sf::Uint16 starty, sf::Uint16 destx, sf::Uint16 desty, const std::vector<Tile> &tiles, sf::Uint16 map_size);

                    bool move(sf::Uint16 &x, sf::Uint16 &y, const std::vector<Tile> &tiles, sf::Uint8 &direction);
                    sf::Uint16 get_map_size() const;

                private:
                    bool do_move(sf::Uint16 &x, sf::Uint16 &y, sf::Uint8 &direction, sf::Uint16 targetx, sf::Uint16 targety, sf::Uint16 speed);

                private:
                    sf::Uint16 m_destx, m_desty, m_map_size;
                    std::deque<sf::Uint16> m_path;
            };
        }
    }
}

#endif
