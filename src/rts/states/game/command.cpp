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
