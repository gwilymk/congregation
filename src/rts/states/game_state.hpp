#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "rts/states/state.hpp"
#include "rts/states/game/command_list.hpp"
#include "rts/states/game/tile.hpp"
#include "rts/network/channel.hpp"
#include "rts/network/server_info.hpp"

#include <SFGUI/Desktop.hpp>
#include <SFGUI/Label.hpp>
#include <vector>
#include <random>

namespace rts
{
    namespace network { class Server; }
    namespace states
    {
        namespace game { class Lobby; }

        class GameState : public State
        {
            enum class CurrentState { InLobby, Waiting, SettingUp, Playing };

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
                void update_input(sf::Time dt);
                void setting_up_update(sf::Time dt);

                void add_to_vertex_array(sf::VertexArray &array, const game::Tile &tile, int id);

            private:
                bool m_lobby_done;
                
                sfg::Desktop m_desktop;
                game::CommandList m_commands;
                game::Lobby *m_lobby;
                network::Server *m_server;
                network::Channel m_channel;
                network::ServerInfo m_info;


                CurrentState m_state;

                sfg::Label::Ptr m_status_label;

                int m_size;
                std::vector<game::Tile> m_tiles;
                std::mt19937 m_random;

                sf::View m_view;
        };
    }
}

#endif
