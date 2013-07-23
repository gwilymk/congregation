#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "rts/states/state.hpp"
#include "rts/states/game/command_list.hpp"
#include "rts/states/game/tile.hpp"

#include <SFGUI/Desktop.hpp>
#include <vector>

namespace rts
{
    namespace states
    {
        class GameState : public State
        {
            public:
                GameState(StateStack &stack, Context context);

                virtual void draw();
                virtual bool update(sf::Time dt);
                virtual bool handle_event(const sf::Event &event);

            private:
                sfg::Desktop m_desktop;
                game::CommandList m_commands;

                std::vector<game::Tile> m_tiles;
        };
    }
}

#endif
