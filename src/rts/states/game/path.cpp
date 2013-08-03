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

#include <cmath>
#include "path.hpp"
#include "rts/states/game/tile.hpp"
#include <map>
#include <set>

namespace rts
{
    namespace states
    {
        namespace game
        {
            /*const sf::Uint16 grass_speed = 1;
            const sf::Uint16 road_speed = 4;
            const sf::Uint16 city_speed = 2;*/

            const sf::Uint16 speeds[3] = {
                10, 40, 20
            };

            const sf::Uint16 costs[3] = {
                8, 2, 4,
            };

            Path::Path(sf::Uint16 map_size):
                m_destx(-1),
                m_desty(-1),
                m_map_size(map_size),
                m_path()
            {
            }

            static bool direction_is_valid(sf::Uint16 id, Tile::Orientation direction, sf::Uint16 m_size)
            {
                sf::Uint16 x = id % m_size;
                sf::Uint16 y = (id - x) / m_size;

                switch(direction) {
                    case Tile::Orientation::NORTH:
                        return y != 0;
                    case Tile::Orientation::EAST:
                        return x < m_size - 1;
                    case Tile::Orientation::SOUTH:
                        return y < m_size - 1;
                    case Tile::Orientation::WEST:
                        return x != 0;
                    default:
                        return false;
                }
            }

            static sf::Uint16 id_in_direction(sf::Uint16 id, Tile::Orientation direction, sf::Uint16 m_size)
            {
                switch(direction) {
                    case Tile::Orientation::NORTH:
                        return id - m_size;
                    case Tile::Orientation::EAST:
                        return id + 1;
                    case Tile::Orientation::SOUTH:
                        return id + m_size;
                    case Tile::Orientation::WEST:
                        return id - 1;
                    default:
                        return -1;
                }
            }

            static sf::Uint16 est_H(sf::Uint16 startid, sf::Uint16 endid, sf::Uint16 map_size)
            {
                sf::Uint16 sx = startid % map_size;
                sf::Uint16 sy = (startid - sx) / map_size;
                sf::Uint16 ex = endid % map_size;
                sf::Uint16 ey = (endid - ex) / map_size;

                return ((sx > ex ? sx - ex : ex - sx) + (sy > ey ? sy - ey : ey - sy)) * 4;
            }

            static std::vector<std::pair<sf::Uint16, Tile::Orientation>> neighbour_tiles(sf::Uint16 tileid, sf::Uint16 m_size)
            {
                std::vector<std::pair<sf::Uint16, Tile::Orientation>> ret;

                for(sf::Uint8 dir = 0; dir < 4; ++dir) {
                    if(direction_is_valid(tileid, (Tile::Orientation) dir, m_size))
                        ret.emplace_back(id_in_direction(tileid, (Tile::Orientation) dir, m_size), (Tile::Orientation) dir);
                }

                return ret;
            }

            Path::Path(sf::Uint16 startx, sf::Uint16 starty, sf::Uint16 destx, sf::Uint16 desty, const std::vector<Tile> &tiles, sf::Uint16 map_size):
                m_destx(destx), 
                m_desty(desty), 
                m_map_size(map_size)
            {
                sf::Uint16 tileid_start = startx / 128 + (starty / 128) * map_size;
                sf::Uint16 tileid_dest = destx / 128 + (desty / 128) * map_size;

                if(tileid_start != tileid_dest) {
                    std::multimap<sf::Uint16, sf::Uint16> f_scores; // f_score, tileid
                    std::map<sf::Uint16, sf::Uint16> g_scores; // tileid, g_score
                    std::map<sf::Uint16, sf::Uint16> came_from;
                    std::set<sf::Uint16> open_set;
                    std::set<sf::Uint16> closed_set;

                    open_set.insert(tileid_start);
                    f_scores.insert(std::make_pair(est_H(tileid_start, tileid_dest, map_size), tileid_start));
                    g_scores[tileid_start] = 0;

                    while(!open_set.empty()) {
                        sf::Uint16 current = 0;
                        for(auto i : f_scores) {
                            if(open_set.find(i.second) != open_set.end()) {
                                current = i.second;
                                break;
                            }
                        }

                        if(current == tileid_dest)
                            break;

                        open_set.erase(current);
                        closed_set.insert(current);

                        for(auto neighbour : neighbour_tiles(current, map_size)) {
                            Tile::Orientation opp_direction = (Tile::Orientation)((neighbour.second + 2) % 4);
                            sf::Uint16 tentative_g_score = g_scores[current] + 
                                costs[tiles[current].get_feature(neighbour.second)] / 2 + 
                                costs[tiles[neighbour.first].get_feature(opp_direction)] / 2;

                            if(closed_set.find(neighbour.first) != closed_set.end()) {
                                if(tentative_g_score >= g_scores[neighbour.first])
                                    continue;
                            }

                            if(open_set.find(neighbour.first) == open_set.end() || tentative_g_score < g_scores[neighbour.first]) {
                                came_from[neighbour.first] = current;
                                g_scores[neighbour.first] = tentative_g_score;
                                f_scores.insert(std::make_pair(tentative_g_score + est_H(neighbour.first, tileid_dest, map_size), neighbour.first));

                                open_set.insert(neighbour.first);
                            }
                        }
                    }

                    sf::Uint16 c = tileid_dest;
                    while(c != tileid_start) {
                        c = came_from[c];
                        if(c != tileid_start)
                            m_path.push_back(c);
                    }
                }
            }

            bool Path::do_move(sf::Uint16 &x, sf::Uint16 &y, sf::Uint8 &direction, sf::Uint16 targetx, sf::Uint16 targety, sf::Uint16 speed)
            {
                if(x == targetx && y == targety)
                    return false;

                if(std::abs(((signed)x) - ((signed)targetx)) <= speed) x = targetx;
                if(std::abs(((signed)y) - ((signed)targety)) <= speed) y = targety;

                if(x > targetx) {
                    x -= speed;
                    direction = 3;
                } else if(x < targetx) {
                    x += speed;
                    direction = 1;
                } else if(y > targety) {
                    y -= speed;
                    direction = 0;
                } else if(y < targety) {
                    y += speed;
                    direction = 2;
                }

                return true;
            }

            bool Path::move(sf::Uint16 &x, sf::Uint16 &y, const std::vector<Tile> &tiles, sf::Uint8 &direction)
            {
                if(m_destx > m_map_size * 128 || m_desty > m_map_size * 128)
                    return false;

                sf::Uint16 speed;
                {
                    sf::Uint16 tileid = (x / 128) + (y / 128) * m_map_size;

                    speed = speeds[tiles[tileid].get_feature(tiles[tileid].get_direction(x % 128, y % 128))];
                }

                if(m_path.empty()) {
                    return do_move(x, y, direction, m_destx, m_desty, speed);
                } else {
                    sf::Uint16 target_tileid = m_path.back();
                    sf::Uint16 targetx = target_tileid % m_map_size;
                    sf::Uint16 targety = (target_tileid - targetx) / m_map_size;
                    if(!do_move(x, y, direction, targetx * 128 + 64, targety * 128 + 64, speed)) {
                        m_path.pop_back();
                    }
                }

                return true;
            }

            sf::Uint16 Path::get_map_size() const
            {
                return m_map_size;
            }
        }
    }
}
