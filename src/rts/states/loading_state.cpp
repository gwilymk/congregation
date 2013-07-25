#include "loading_state.hpp"
#include "rts/state_stack.hpp"
#include "rts/states/id.hpp"

namespace rts 
{
    namespace states
    {
        LoadingState::LoadingState(StateStack &stack, Context context):
            State(stack, context),
            m_lua_state()
        {
            m_lua_state.new_table();

            m_lua_state.push<std::function<int(lua_State *)>>(
            [this] (lua_State *L_raw) -> int {
                lua::State L(L_raw);
                ASSERT(L.get_top() == 2);
                std::string name = L.get<std::string>(1);
                std::string fname = L.get<std::string>(2);

                get_context().texture_holder->load(name, fname);
                return 0;
            });
            m_lua_state.set_field(-2, "load_texture");

            m_lua_state.push<std::function<int(lua_State *)>>(
            [this] (lua_State *L_raw) -> int {
                lua::State L(L_raw);
                ASSERT(L.get_top() == 2);
                std::string name = L.get<std::string>(1);
                std::string fname = L.get<std::string>(2);

                get_context().font_holder->load(name, fname);
                return 0;
            });
            m_lua_state.set_field(-2, "load_font");

            m_lua_state.set_global("rts");
        }

        bool LoadingState::update(sf::Time)
        {
            m_lua_state.run_file("../res/init.lua");
            request_stack_pop();
            request_stack_push(ID::TitleState);
            return true;
        }

        void LoadingState::draw()
        {
        }

        bool LoadingState::handle_event(const sf::Event &)
        {
            return true;
        }
    }
}
