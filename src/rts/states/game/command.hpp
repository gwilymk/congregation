#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vector>
#include <SFML/Config.hpp>

#include <SFML/Network/Packet.hpp>

namespace rts
{
    namespace states
    {
        namespace game
        {
            struct Command
            {
                enum COMMAND { PlacePiece, MoveUnits };
                sf::Uint8 type;

                union {
                    struct {
                        sf::Uint8 x;
                        sf::Uint8 y;
                        sf::Uint8 tileid;
                    } piece_place;
                    struct {
                        std::vector<sf::Uint16> to_move;
                        sf::Uint8 x;
                        sf::Uint8 y;
                    } unit_move;
                } data;
            };

            sf::Packet &operator<<(sf::Packet &packet, const Command &command);
            sf::Packet &operator>>(sf::Packet &packet, Command &command);
        }
    }
}

#endif
