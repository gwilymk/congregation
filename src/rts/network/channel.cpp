#include "channel.hpp"
#include <SFML/Network.hpp>

namespace rts
{
    namespace network
    {
        Channel::Channel():
            m_accepting(false)
        {
            m_listener.setBlocking(false);
        }

        Channel::~Channel()
        {
        }

        void Channel::add_peer(const sf::IpAddress &peer_address)
        {
            sf::TcpSocket *s = new sf::TcpSocket();
            assert(s->connect(peer_address, network::peer_listen_port) == sf::TcpSocket::Done);
            s->setBlocking(false);
            m_peers.emplace_back(s);
        }

        void Channel::connect_to_server(const sf::IpAddress &server_address)
        {
            assert(m_server_socket.connect(server_address, network::listen_port) == sf::TcpSocket::Done);
            sf::Packet query;
            query << network::want_connection;
            assert(m_server_socket.send(query) == sf::Socket::Done);
            m_server_socket.setBlocking(false);
        }

        void Channel::update()
        {
            if(m_accepting) {
                std::unique_ptr<sf::TcpSocket> s(new sf::TcpSocket());
                if(m_listener.accept(*s) == sf::Socket::Done) {
                    m_peers.push_back(std::move(s));
                }
            } else if(m_peers.empty()) {
                sf::Packet packet;
                if(m_server_socket.receive(packet) == sf::Socket::Done) {
                    packet >> m_server_info;
                }
            }
        }

        int Channel::num_players()
        {
            return m_server_info.num_players;
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
            m_listener.listen(network::peer_listen_port);
        }
    }
}
