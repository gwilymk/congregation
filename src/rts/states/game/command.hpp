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
                } place_piece;
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
