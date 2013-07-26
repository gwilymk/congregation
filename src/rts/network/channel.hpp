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
