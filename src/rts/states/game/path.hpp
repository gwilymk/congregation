#ifndef PATH_HPP
#define PATH_HPP

#include <vector>
#include <SFML/Config.hpp>

namespace rts
{
    namespace states
    {
        namespace game
        {
            class Path
            {
                public:
                    Path();
                    Path(sf::Uint16 destx, sf::Uint16 desty);

                    bool move(sf::Uint16 &x, sf::Uint16 &y, sf::Uint8 dist, sf::Uint8 &direction);

                private:
                    sf::Uint16 m_destx, m_desty;
            };
        }
    }
}

#endif
