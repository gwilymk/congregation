#ifndef SERVER_HPP
#define SERVER_HPP

#include <SFML/System/NonCopyable.hpp>

#include "rts/network/server_info.hpp"
#include <memory>

namespace rts
{
    namespace network
    {
        class Server : private sf::NonCopyable
        {
            struct Impl;

            public:
                explicit Server(const ServerInfo &info);
                ~Server();

                void listen();

            private:
                void update_info();

            private:
                ServerInfo m_info;
                std::unique_ptr<Impl> m_impl;
        };
    }
}

#endif
