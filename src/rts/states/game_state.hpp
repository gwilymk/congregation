#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "rts/states/state.hpp"
#include "rts/states/game/command_list.hpp"
#include "rts/states/game/tile.hpp"

#include <SFGUI/Desktop.hpp>
#include <vector>

namespace rts
{
    namespace network { class Server; }
    namespace states
    {
        namespace game { class Lobby; }

        class GameState : public State
        {
            enum class CurrentState { InLobby, Waiting, Playing };

            public:
                GameState(StateStack &stack, Context context);
                ~GameState();

                virtual void draw();
                virtual bool update(sf::Time dt);
                virtual bool handle_event(const sf::Event &event);

            private:
                void lobby_update(sf::Time dt);
                void waiting_update(sf::Time dt);
                void playing_update(sf::Time dt);

            private:
                bool m_lobby_done;
                
                sfg::Desktop m_desktop;
                game::CommandList m_commands;
                game::Lobby *m_lobby;
                network::Server *m_server;

                CurrentState m_state;

                std::vector<game::Tile> m_tiles;
        };
    }
}

#endif
