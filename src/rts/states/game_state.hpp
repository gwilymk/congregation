#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "rts/states/state.hpp"
#include "rts/states/game/command_list.hpp"
#include "rts/states/game/tile.hpp"
#include "rts/states/game/minion.hpp"
#include "rts/network/channel.hpp"
#include "rts/network/server_info.hpp"

#include <SFML/Graphics/Sprite.hpp>
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

                void add_to_vertex_array(sf::VertexArray &array, int id);

                void create_minion(sf::Uint16 x, sf::Uint16 y, sf::Uint8 player_num);
                void kill_minion(sf::Uint16 id);

                void send_command(game::Command command);
                bool legal_move(game::Tile tile, sf::Uint16 x, sf::Uint16 y);

                game::Tile &get_tile(sf::Uint16 x, sf::Uint16 y);
                bool check_city(sf::Uint16 x, sf::Uint16 y);
                bool check_city(sf::Uint16 x, sf::Uint16 y, game::Tile::Orientation direction);

                bool direction_is_valid(sf::Uint16 x, sf::Uint16 y, game::Tile::Orientation direction);
                game::Tile &tile_in_direction(sf::Uint16 x, sf::Uint16 y, game::Tile::Orientation direction);

            private:
                bool m_lobby_done;
                
                game::CommandList m_commands;
                game::Lobby *m_lobby;
                network::Server *m_server;
                network::Channel m_channel;
                network::ServerInfo m_info;
                sf::Time m_update_time;
                bool m_finished_current_turn = false;
                std::vector<sf::Uint32> m_player_turns;

                CurrentState m_state;

                sfg::Label::Ptr m_status_label;

                int m_size;
                bool m_selecting;
                sf::Vector2f m_select_start;
                sf::Vector2f m_select_end;
                std::vector<game::Tile> m_tiles;
                std::vector<game::Minion> m_minions;
                std::vector<sf::Uint16> m_my_minions;
                std::vector<sf::Color> m_player_colours;
                std::vector<std::vector<sf::Uint16>> m_minion_tiles;
                std::vector<sf::Uint8> m_visibility;
                std::set<int> m_free_list;
                std::mt19937 m_random;

                std::mt19937 m_tile_random;
                std::uniform_int_distribution<sf::Uint8> m_tile_dist;
                game::Tile m_next_tile;
                sf::Sprite m_next_tile_sprite;
                bool m_placing_tile = false;

                sf::Uint8 m_my_player;

                sf::Sprite m_selected_sprite;
                sf::Sprite m_minion_counter_sprite;
                sf::Sprite m_tile_placement_box_sprite;
                sf::Sprite m_hud_background_sprite;
                sf::Text m_minion_counter_text;
                sf::Shader *m_minion_counter_shader;

                sf::View m_view;

        };
    }
}

#endif
