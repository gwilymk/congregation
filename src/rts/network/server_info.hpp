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
        const int max_players = 8;

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
