#ifndef SERVER_INFO_HPP
#define SERVER_INFO_HPP

#include <SFML/Config.hpp>
#include <SFML/Network/Packet.hpp>

#include <string>
#include <vector>

namespace rts
{
    namespace network
    {
        const std::vector<std::string> sizes = { "Small", "Medium", "Large" };
        const sf::Uint8 map_sizes[] = { 32, 48, 64 };
        const int max_players = 8;

        const std::string has_server_message = "server?";
        const std::string reply_message = "me!";
        const std::string want_info = "info please";
        const std::string want_connection = "connection please";
        const std::string info_update = "info update";

        const unsigned short broadcast_port = 10037;
        const unsigned short listen_port = 10038;
        const unsigned short peer_listen_port = 10039;

        struct ServerInfo
        {
            sf::Uint8 map_size;
            sf::Uint8 num_players;
            sf::Uint8 max_players;
            sf::Uint16 version;
        };

        sf::Packet &operator<<(sf::Packet &packet, const ServerInfo &info);
        sf::Packet &operator>>(sf::Packet &packet, ServerInfo &info);
    }
}

#endif
