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

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "rts/common.hpp"
#include "rts/network/server_info.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <array>
#include <vector>
#include <memory>

namespace rts
{
    namespace network
    {
        const std::string next_turn = "ready for turn";
        const std::string add_command = "add command";

        class Channel : private sf::NonCopyable
        {
            public:
                explicit Channel(bool *done);
                ~Channel();

                void connect_to_server(const sf::IpAddress &server_address, unsigned short port);

                void update();
                int num_players();
                int max_players();
                sf::Uint8 get_my_player() const;

                sf::Socket::Status receive(sf::Packet &packet, sf::Uint8 &player);
                sf::Socket::Status send(sf::Packet &packet);

                std::array<sf::Uint32, network::seed_size> get_seed() const;
                ServerInfo get_server_info() const;

            private:
                void start_accepting_peers();
                void stop_accepting_peers();
                void add_peer(const sf::IpAddress &peer_address, unsigned short port, sf::Uint8 player_num);

                void get_peer();

            private:
                sf::TcpListener m_listener;
                sf::TcpSocket m_server_socket;
                bool m_accepting;
                bool *m_done;
                sf::Uint8 m_my_player;
                std::array<sf::Uint32, network::seed_size> m_seed;

                ServerInfo m_server_info;

                std::vector<std::pair<sf::Uint8, std::unique_ptr<sf::TcpSocket>>> m_peers;
        };
    }
}

#endif
