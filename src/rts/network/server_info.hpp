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
        const int seed_size = 5;

        const std::string has_server_message = "server?";
        const std::string reply_message = "me!";
        const std::string want_info = "info please";
        const std::string want_connection = "connection please";
        const std::string info_update = "info update";
        const std::string start_game = "start game!";
        const std::string new_peer = "new peer";

        const unsigned short broadcast_port = 10037;
        const unsigned short listen_port = 10038;

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
