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

#include "channel.hpp"
#include <SFML/Network.hpp>

#include <iostream>

namespace rts
{
    namespace network
    {
        Channel::Channel(bool *done):
            m_listener(),
            m_accepting(false),
            m_done(done),
            m_my_player(149)
        {
            m_listener.setBlocking(false);
            *m_done = false;
        }

        Channel::~Channel()
        {
        }

        void Channel::add_peer(const sf::IpAddress &peer_address, unsigned short port, sf::Uint8 player_num)
        {
            std::unique_ptr<sf::TcpSocket> s(new sf::TcpSocket());
            std::cerr << "Connecting to " << peer_address.toString() << ":" << port << " (will be player " << (unsigned) player_num << ")\n";
            ASSERT(s->connect(peer_address, port, sf::seconds(3.0f)) == sf::Socket::Done);
            sf::Packet greeting;
            greeting << m_my_player;
            s->send(greeting);
            s->setBlocking(false);
            m_peers.push_back(std::make_pair(player_num, std::move(s)));
        }

        void Channel::connect_to_server(const sf::IpAddress &server_address, unsigned short port)
        {
            ASSERT(m_server_socket.connect(server_address, port) == sf::Socket::Done);
            start_accepting_peers();
            sf::Packet query;
            query << network::want_connection;
            query << (sf::Uint16) m_listener.getLocalPort();
            ASSERT(m_server_socket.send(query) == sf::Socket::Done);
            sf::Packet reply;
            ASSERT(m_server_socket.receive(reply) == sf::Socket::Done);
            reply >> m_my_player >> m_server_info;

            std::cerr << "I am player: " << (unsigned) m_my_player << std::endl;
            m_server_socket.setBlocking(false);
        }

        void Channel::update()
        {
            if(m_accepting) {
                get_peer();
            } 
            
            sf::Packet packet;
            if(m_server_socket.receive(packet) == sf::Socket::Done) {
                std::string command;
                packet >> command;

                std::cerr << "Channel got command: " << command << std::endl;
                if(command == info_update) {
                    packet >> m_server_info;
                } else if(command == start_game) {
                    for(int i = 0; i < seed_size; ++i)
                        packet >> m_seed[i];

                    m_server_socket.disconnect();
                    while(num_players() != max_players()) {
                        m_listener.setBlocking(true);
                        get_peer();
                    }
                    stop_accepting_peers();
                    *m_done = true;
                } else if(command == new_peer) {
                    sf::Uint32 address;
                    packet >> address;
                    sf::Uint16 port_number;
                    packet >> port_number;
                    sf::Uint8 player_num;
                    packet >> player_num;
                    add_peer(sf::IpAddress(address), port_number, player_num);
                }
            }
        }

        int Channel::num_players()
        {
            return m_peers.size() + 1;
        }

        int Channel::max_players()
        {
            return m_server_info.max_players;
        }

        void Channel::get_peer()
        {
            std::unique_ptr<sf::TcpSocket> s(new sf::TcpSocket());
            if(m_listener.accept(*s) == sf::Socket::Done) {
                sf::Packet greeting;
                s->receive(greeting);
                sf::Uint8 player_num_of_peer;
                greeting >> player_num_of_peer;
                std::cerr << "Got peer " << s->getRemoteAddress().toString() << " who is player " << (unsigned) player_num_of_peer << std::endl;
                s->setBlocking(false);
                m_peers.push_back(std::make_pair(player_num_of_peer, std::move(s)));
            }
        }

        void Channel::stop_accepting_peers()
        {
            m_accepting = false;
            m_listener.close();
        }

        void Channel::start_accepting_peers()
        {
            m_accepting = true;
            m_listener.listen(sf::Socket::AnyPort);
            std::cerr << "Starting to listen on port " << (sf::Uint16) m_listener.getLocalPort() << std::endl;
        }

        sf::Uint8 Channel::get_my_player() const
        {
            return m_my_player;
        }

        sf::Socket::Status Channel::receive(sf::Packet &packet, sf::Uint8 &player)
        {
            sf::Socket::Status ret = sf::Socket::Error;
            for(auto peer = m_peers.begin() ; peer != m_peers.end() ; ++peer) {
                ret = peer->second->receive(packet);
                ASSERT(ret != sf::Socket::Error);
                player = peer->first;
                if(ret == sf::Socket::Done) return ret;
            }

            return ret;
        }

        sf::Socket::Status Channel::send(sf::Packet &packet)
        {
            for(auto peer = m_peers.begin() ; peer != m_peers.end() ; ++peer) {
                sf::Socket::Status status = sf::Socket::NotReady;
                while(status == sf::Socket::NotReady)
                    status = peer->second->send(packet);
                    
                if(status != sf::Socket::Done) return status;
            }

            return sf::Socket::Done;
        }

        std::array<sf::Uint32, network::seed_size> Channel::get_seed() const
        {
            return m_seed;
        }

        ServerInfo Channel::get_server_info() const
        {
            return m_server_info;
        }
    }
}
