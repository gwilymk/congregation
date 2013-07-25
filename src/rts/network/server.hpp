#ifndef SERVER_HPP
#define SERVER_HPP

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Mutex.hpp>

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
                unsigned short port();

            private:
                void run();
                void new_peer(const sf::IpAddress &address, sf::Uint16 port);

            private:
                ServerInfo m_info;
                std::unique_ptr<Impl> m_impl;
                bool started;

                bool stop;

                sf::Thread m_thread;
                sf::Mutex m_mutex;
        };
    }
}

#endif
