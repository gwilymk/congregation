#ifndef LOBBY_HPP
#define LOBBY_HPP

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
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

                public:
                    Lobby(bool *done);
                    ~Lobby();

                    void add_to_desktop(sfg::Desktop &desktop);
                    void update_server_list();
                    void create_server();
                    void update(sf::Time dt);

                    bool server() const;
                    network::ServerInfo get_server_info() const;

                private:
                    void server_update(sf::Time dt);
                    void client_update(sf::Time dt);

                private:
                    bool *m_done;
                    std::unique_ptr<Impl> m_impl;

                    bool m_server;
            };
        }
    }
}

#endif
