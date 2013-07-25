#include "server.hpp"
#include <SFML/Network.hpp>
#include "rts/common.hpp"
#include <vector>
#include <memory>
#include <random>

#include <iostream>

namespace rts
{
    namespace network
    {
        struct Server::Impl
        {
            Impl():
                broadcast_socket(),
                listener(),
                client(),
                client_list()
            {}

            sf::UdpSocket broadcast_socket;
            sf::TcpListener listener;
            sf::TcpSocket client;

            std::vector<std::unique_ptr<sf::TcpSocket>> client_list;
        };

        Server::Server(const ServerInfo &info):
            m_info(info),
            m_impl(new Impl()),
            started(false)
        {
            m_impl->broadcast_socket.setBlocking(false);
            m_impl->listener.setBlocking(false);
            m_impl->broadcast_socket.bind(network::broadcast_port);
            m_impl->listener.listen(sf::Socket::AnyPort);
        }

        Server::~Server()
        {
        }

        unsigned short Server::port()
        {
            return m_impl->listener.getLocalPort();
        }

        void Server::listen()
        {
            sf::Packet packet;
            sf::IpAddress addr;
            unsigned short port;
            if(m_impl->broadcast_socket.receive(packet, addr, port) == sf::Socket::Done) {
                std::string message;
                packet >> message;
                if(message == network::has_server_message) {
                    std::cerr << "Yay! " << addr.toString() << ":" << port << std::endl;
                    sf::Packet reply;
                    reply << network::reply_message;
                    reply << m_impl->listener.getLocalPort();
                    ASSERT(m_impl->broadcast_socket.send(reply, addr, port) == sf::Socket::Done);
                }
            }

            std::unique_ptr<sf::TcpSocket> c(new sf::TcpSocket());
            
            if(m_impl->listener.accept(*c) == sf::Socket::Done) {
                sf::Packet packet;
                std::string command;
                c->receive(packet);
                packet >> command;

                std::cerr << "Server got command: " << command << std::endl;
                if(command == want_info) {
                    sf::Packet reply;
                    reply << m_info;
                    ASSERT(c->send(reply) == sf::Socket::Done);
                    c->disconnect();
                } else if(command == want_connection) {
                    sf::Uint16 port;
                    packet >> port;
                    new_peer(c->getRemoteAddress(), port);

                    c->setBlocking(false);
                    m_impl->client_list.push_back(std::move(c));
                    m_info.num_players = m_impl->client_list.size();
                } else {
                    ASSERT(false);
                }
            }

            packet.clear();
            for(auto client = m_impl->client_list.begin() ; client != m_impl->client_list.end() ; ++client) {
                if((*client)->receive(packet) == sf::Socket::Done) {
                    std::string command;
                    packet >> command;
                    if(command == want_info) {
                        sf::Packet reply;
                        reply << info_update;
                        reply << m_info;
                        ASSERT((*client)->send(reply) == sf::Socket::Done);
                    }
                }
            }

            if(!started && m_info.num_players == m_info.max_players) {
                sf::Uint8 player_num = 0;
                sf::Uint32 seed[seed_size];
                std::random_device rd;
                for(int i = 0; i < seed_size; ++i) 
                    seed[i] = rd();

                for(auto client = m_impl->client_list.begin() ; client != m_impl->client_list.end() ; ++client) {
                    sf::Packet message;
                    message << network::start_game;
                    message << player_num;
                    for(int i = 0; i < seed_size; ++i) 
                        message << seed[i];
                    ASSERT((*client)->send(message) == sf::Socket::Done);
                    ++player_num;
                }

                started = true;
            }
        }

        void Server::new_peer(const sf::IpAddress &address, sf::Uint16 port_num)
        {
            sf::Packet packet;
            packet << network::new_peer;
            packet << address.toInteger() << port_num;
            packet << network::new_peer;

            for(auto client = m_impl->client_list.begin() ; client != m_impl->client_list.end() ; ++client) {
                ASSERT((*client)->send(packet) == sf::Socket::Done);
            }
        }
    }
}
