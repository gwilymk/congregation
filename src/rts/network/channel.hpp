#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "rts/common.hpp"
#include "rts/network/server_info.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <vector>
#include <memory>

namespace rts
{
    namespace network
    {
        class Channel : private sf::NonCopyable
        {
            public:
                Channel();
                ~Channel();

                void connect_to_server(const sf::IpAddress &server_address);
                void add_peer(const sf::IpAddress &peer_address);

                void update();
                void start_accepting_peers();
                void stop_accepting_peers();

            private:
                sf::TcpListener m_listener;
                sf::TcpSocket m_server_socket;
                bool m_accepting;

                std::vector<std::unique_ptr<sf::TcpSocket>> m_peers;
        };
    }
}

#endif
