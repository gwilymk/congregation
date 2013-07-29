/*
    Congregation - An rts game involving the placement of tiles
    Copyright (C) 2013  Gwilym Kuiper (kuiper.gwilym@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
