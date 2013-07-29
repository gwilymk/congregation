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
