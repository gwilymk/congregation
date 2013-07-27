#include "command.hpp"
#include <iostream>

#include <stdexcept>

namespace rts
{
    namespace states
    {
        namespace game
        {
            bool Command::less(const Command &a, const Command &b)
            {
                if(a.turn == b.turn)
                    return a.player < b.player;
                else
                    return a.turn < b.turn;
            }

            sf::Packet &operator<<(sf::Packet &packet, const Command &command)
            {
                packet << command.type;
                packet << command.player;
                packet << command.turn;

                switch(command.type) {
                    case Command::COMMAND::PlacePiece:
                        packet << command.place_piece.x << command.place_piece.y << command.place_piece.tile;
                        break;
                        
                    case Command::COMMAND::MoveUnits:
                        sf::Uint16 size;

                        size = command.unit_move.to_move.size();
                        packet << size;
                        
                        for(sf::Uint16 id : command.unit_move.to_move) {
                            packet << id;
                        }

                        packet << command.unit_move.x << command.unit_move.y;
                        break;

                    default:
                        throw std::runtime_error("Unknown packet type: " + command.type);
                }

                return packet;
            }

            sf::Packet &operator>>(sf::Packet &packet, Command &command)
            {
                packet >> command.type;
                packet >> command.player;
                packet >> command.turn;

                switch(command.type) {
                    case Command::COMMAND::PlacePiece:
                        packet >> command.place_piece.x >> command.place_piece.y >> command.place_piece.tile; 
                        break;

                    case Command::COMMAND::MoveUnits:
                        sf::Uint16 size;
                        packet >> size;
                        command.unit_move.to_move = std::vector<sf::Uint16>();

                        for(sf::Uint16 i = 0 ; i < size ; ++i) {
                            sf::Uint16 tmp;
                            packet >> tmp;
                            command.unit_move.to_move.push_back(tmp);
                        }

                        packet >> command.unit_move.x >> command.unit_move.y;
                        break;

                    default:
                        throw std::runtime_error("Unknown packet type: " + command.type);
                }

                return packet;
            }
        }
    }
}
