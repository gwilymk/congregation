#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "rts/states/game/tile.hpp"

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
                enum COMMAND { PlacePiece, MoveUnits, Invalid };

                sf::Uint8 type = COMMAND::Invalid;
                sf::Uint8 player;
                sf::Uint32 turn;

                struct {
                    sf::Uint8 x;
                    sf::Uint8 y;
                    Tile tile;
                } piece_place;
                struct {
                    std::vector<sf::Uint16> to_move;
                    sf::Uint16 x;
                    sf::Uint16 y;
                } unit_move;

                static bool less(const Command &a, const Command &b);
            };


            sf::Packet &operator<<(sf::Packet &packet, const Command &command);
            sf::Packet &operator>>(sf::Packet &packet, Command &command);
        }
    }
}

#endif
