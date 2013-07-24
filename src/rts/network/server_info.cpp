#include "server_info.hpp"

namespace rts
{
    namespace network
    {
        sf::Packet &operator<<(sf::Packet &packet, const ServerInfo &info)
        {
            return packet << info.map_size << info.num_players << info.max_players << info.version;
        }

        sf::Packet &operator>>(sf::Packet &packet, ServerInfo &info)
        {
            return packet >> info.map_size >> info.num_players >> info.max_players >> info.version;
        }
    }
}
