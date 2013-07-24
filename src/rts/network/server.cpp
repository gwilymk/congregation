#include "server.hpp"
#include <SFML/Network.hpp>
#include "rts/common.hpp"

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
            
            if(m_impl->listener.accept(m_impl->client) == sf::Socket::Done) {
                sf::Packet packet;
                packet << m_info;
                assert(m_impl->client.send(packet) == sf::Socket::Done);
                m_impl->client.disconnect();
            }
        }
    }
}
