#include <cmath>
#include "path.hpp"
#include "rts/states/game/tile.hpp"

namespace rts
{
    namespace states
    {
        namespace game
        {
            Path::Path():
                m_destx(0),
                m_desty(0)
            {
            }

            Path::Path(sf::Uint16 destx, sf::Uint16 desty):
                m_destx(destx), 
                m_desty(desty)
            {
            }

            bool Path::move(sf::Uint16 &x, sf::Uint16 &y, sf::Uint8 dist, sf::Uint8 &direction)
            {
                if(m_destx == 0 && m_desty == 0) return false;

                if(x != m_destx) {
                    if(x > m_destx) {
                        x -= dist;
                        direction = Tile::Orientation::WEST;
                    } else {
                        x += dist;
                        direction = Tile::Orientation::EAST;
                    }

                    if(std::abs((signed)x - (signed)m_destx) < dist)
                        x = m_destx;
                } else if(y != m_desty) {
                    if(y > m_desty) {
                        y -= dist;
                        direction = Tile::Orientation::NORTH;
                    } else {
                        y += dist;
                        direction = Tile::Orientation::SOUTH;
                    }

                    if(std::abs((signed)y - (signed)m_desty) < dist)
                        y = m_desty;
                } else
                    return false;
                return true;
            }
        }
    }
}
