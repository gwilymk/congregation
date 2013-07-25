#include "channel.hpp"
#include <SFML/Network.hpp>

#include <iostream>

namespace rts
{
    namespace network
    {
        Channel::Channel():
            m_listener(),
            m_accepting(false),
            m_my_player(149)
        {
            m_listener.setBlocking(false);
        }

        Channel::~Channel()
        {
        }

        void Channel::add_peer(const sf::IpAddress &peer_address, unsigned short port)
        {
            sf::TcpSocket *s = new sf::TcpSocket();
            std::cerr << "Connecting to " << peer_address.toString() << ":" << port << std::endl;
            ASSERT(s->connect(peer_address, port) == sf::Socket::Done);
            s->setBlocking(false);
            m_peers.emplace_back(s);
        }

        void Channel::connect_to_server(const sf::IpAddress &server_address, unsigned short port)
        {
            ASSERT(m_server_socket.connect(server_address, port) == sf::Socket::Done);
            sf::Packet query;
            query << network::want_connection;
            start_accepting_peers();
            query << (sf::Uint16) m_listener.getLocalPort();
            ASSERT(m_server_socket.send(query) == sf::Socket::Done);
            query.clear();
            query << network::want_info;
            ASSERT(m_server_socket.send(query) == sf::Socket::Done);
            m_server_socket.setBlocking(false);
        }

        void Channel::update()
        {
            if(m_accepting) {
                std::unique_ptr<sf::TcpSocket> s(new sf::TcpSocket());
                if(m_listener.accept(*s) == sf::Socket::Done) {
                    std::cerr << "Got peer " << s->getRemoteAddress().toString() << std::endl;
                    m_peers.push_back(std::move(s));
                }
            } 
            
            sf::Packet packet;
            if(m_server_socket.receive(packet) == sf::Socket::Done) {
                std::string command;
                packet >> command;

                std::cerr << "Channel got command: " << command << std::endl;
                if(command == info_update) {
                    packet >> m_server_info;
                } else if(command == start_game) {
                    packet >> m_my_player;
                    std::cerr << "I am player: " << (unsigned) m_my_player << std::endl;
                    m_server_socket.disconnect();
                    m_listener.close();
                } else if(command == new_peer) {
                    sf::Uint32 address;
                    packet >> address;
                    sf::Uint16 port_number;
                    packet >> port_number;
                    add_peer(sf::IpAddress(address), port_number);
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
    }
}
