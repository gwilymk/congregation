#ifndef LOBBY_HPP
#define LOBBY_HPP

#include <SFML/System/NonCopyable.hpp>
#include <memory>

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
                    void update();

                private:
                    bool *m_done;
                    std::unique_ptr<Impl> m_impl;

                    bool server;
            };
        }
    }
}

#endif
