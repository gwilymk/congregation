#include "command.hpp"

#include <stdexcept>

namespace rts
{
    namespace states
    {
        namespace game
        {
            sf::Packet &operator<<(sf::Packet &packet, const Command &command)
            {
                packet << command.type;
                switch(command.type) {
                    case Command::COMMAND::PlacePiece:
                        packet << command.data.piece_place.x << command.data.piece_place.y << command.data.piece_place.tileid;
                        break;
                        
                    case Command::COMMAND::MoveUnits:
                        packet << (sf::Uint16) command.data.unit_move.to_move.size();
                        
                        for(sf::Uint16 id : command.data.unit_move.to_move) {
                            packet << id;
                        }

                        packet << command.data.unit_move.x << command.data.unit_move.y;
                        break;

                    default:
                        throw std::runtime_error("Unknown packet type: " + command.type);
                }

                return packet;
            }

            sf::Packet &operator>>(sf::Packet &packet, Command &command)
            {
                packet >> command.type;

                switch(command.type) {
                    case Command::COMMAND::PlacePiece:
                        packet >> command.data.piece_place.x >> command.data.piece_place.y >> command.data.piece_place.tileid; 
                        break;

                    case Command::COMMAND::MoveUnits:
                        sf::Uint16 size;
                        packet >> size;
                        command.data.unit_move.to_move = std::vector<sf::Uint16>();
                        command.data.unit_move.to_move.reserve(size);

                        for(sf::Int16 i = 0 ; i < size ; ++i) {
                            packet >> command.data.unit_move.to_move[i];
                        }

                        packet >> command.data.unit_move.x >> command.data.unit_move.y;
                        break;

                    default:
                        throw std::runtime_error("Unknown packet type: " + command.type);
                }

                return packet;
            }
        }
    }
}
