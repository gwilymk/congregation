#include "server.hpp"
#include <SFML/System/Lock.hpp>
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
            started(false),
            stop(false),
            m_thread(&Server::run, this)
        {
            m_impl->broadcast_socket.setBlocking(false);
            m_impl->listener.setBlocking(false);
            m_impl->broadcast_socket.bind(network::broadcast_port);
            m_impl->listener.listen(sf::Socket::AnyPort);
        }

        Server::~Server()
        {
            m_mutex.lock();
            stop = true;
            m_mutex.unlock();
            m_thread.wait();
        }

        unsigned short Server::port()
        {
            sf::Lock lock(m_mutex);
            return m_impl->listener.getLocalPort();
        }

        void Server::listen()
        {
            stop = false;
            m_thread.launch();
        }

        void Server::run()
        {
            while(true) {
                m_mutex.lock();
                if(started || stop) {
                    m_mutex.unlock();   
                    break;
                }

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
                    sf::Packet reply;
                    std::string command;
                    c->receive(packet);
                    packet >> command;

                    std::cerr << "Server got command: " << command << std::endl;
                    if(command == want_info) {
                        reply << m_info;
                        ASSERT(c->send(reply) == sf::Socket::Done);
                        c->disconnect();
                    } else if(command == want_connection) {
                        sf::Uint16 port;
                        packet >> port;

                        reply << (sf::Uint8) m_impl->client_list.size() << m_info;
                        ASSERT(c->send(reply) == sf::Socket::Done);
                        c->setBlocking(false);
                        new_peer(c->getRemoteAddress(), port);
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

                if(m_info.num_players == m_info.max_players) {
                    sf::Uint32 seed[seed_size];
                    std::random_device rd;
                    for(int i = 0; i < seed_size; ++i) 
                        seed[i] = rd();

                    sf::Packet message;
                    message << network::start_game;
                    for(int i = 0; i < seed_size; ++i) 
                        message << seed[i];

                    for(auto client = m_impl->client_list.begin() ; client != m_impl->client_list.end() ; ++client) {
                        ASSERT((*client)->send(message) == sf::Socket::Done);
                    }

                    started = true;
                }
                m_mutex.unlock();
                sf::sleep(sf::seconds(0.1f));
            }
        }

        void Server::new_peer(const sf::IpAddress &address, sf::Uint16 port_num)
        {
            sf::Packet packet;
            packet << network::new_peer;
            packet << address.toInteger() << port_num;
            packet << (sf::Uint8) m_impl->client_list.size();

            for(auto client = m_impl->client_list.begin() ; client != m_impl->client_list.end() ; ++client) {
                ASSERT((*client)->send(packet) == sf::Socket::Done);
            }
        }
    }
}
