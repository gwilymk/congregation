#include "server.hpp"
#include <SFML/Network.hpp>
#include "rts/common.hpp"
#include <vector>
#include <memory>

#include <iostream>

namespace rts
{
    namespace network
    {
        struct Server::Impl
        {
            sf::UdpSocket broadcast_socket;
            sf::TcpListener listener;
            sf::TcpSocket client;

            std::vector<std::unique_ptr<sf::TcpSocket>> client_list;
        };

        Server::Server(const ServerInfo &info):
            m_info(info),
            m_impl(new Impl())
        {
            m_impl->broadcast_socket.setBlocking(false);
            m_impl->listener.setBlocking(false);
            m_impl->broadcast_socket.bind(network::broadcast_port);
            m_impl->listener.listen(network::listen_port);
        }

        Server::~Server()
        {
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
                    assert(m_impl->broadcast_socket.send(reply, addr, port) == sf::Socket::Done);
                }
            }

            std::unique_ptr<sf::TcpSocket> c(new sf::TcpSocket());
            
            if(m_impl->listener.accept(*c) == sf::Socket::Done) {
                sf::Packet packet;
                std::string command;
                c->receive(packet);
                packet >> command;
                sf::Packet reply;

                std::cerr << command << std::endl;
                if(command == want_info) {
                    reply << m_info;
                    assert(c->send(packet) == sf::Socket::Done);
                } else if(command == want_connection) {
                    m_impl->client_list.push_back(std::move(c));
                    m_info.num_players = m_impl->client_list.size();
                 
                    update_info();
                }
            }
        }

        void Server::update_info()
        {
            sf::Packet message;
            message << network::info_update << m_info;

            for(auto client = m_impl->client_list.begin() ; client != m_impl->client_list.end() ; ++client) {
                assert((*client)->send(message) == sf::Socket::Done);
            }
        }
    }
}
