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

#ifndef LOBBY_HPP
#define LOBBY_HPP

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include <SFML/Network/IpAddress.hpp>
#include <memory>

#include "rts/network/server_info.hpp"

namespace sfg
{
    class Desktop;
}

namespace rts
{
    namespace states
    {
        namespace game
        {
            class Lobby : private sf::NonCopyable
            {
                struct Impl;
                struct ButtonCallback;

                public:
                    Lobby(bool *done);
                    ~Lobby();

                    void add_to_desktop(sfg::Desktop &desktop);
                    void update_server_list();
                    void create_server();
                    void update(sf::Time dt);

                    bool server() const;
                    network::ServerInfo get_server_info() const;

                    sf::IpAddress get_server() const;
                    unsigned short get_server_port() const;

                private:
                    void server_update(sf::Time dt);
                    void client_update(sf::Time dt);

                    void set_server(int serverid);

                    void quit();

                private:
                    bool *m_done;
                    std::unique_ptr<Impl> m_impl;

                    bool m_server;
                    int m_serverid;
            };
        }
    }
}

#endif
