#include "loading_state.hpp"
#include "rts/state_stack.hpp"
#include "rts/states/id.hpp"
#include "rts/holders/music_holder.hpp"
#include <SFGUI/Desktop.hpp>

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
                ASSERT(L.get_top() == 3);
                std::string name = L.get<std::string>(1);
                std::string fname = L.get<std::string>(2);
                bool smooth = L.get<bool>(3);

                get_context().texture_holder->load(name, fname);
                get_context().texture_holder->get(name).setSmooth(smooth);
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

            m_lua_state.push<std::function<int(lua_State *)>>(
            [this] (lua_State *L_raw) -> int {
                lua::State L(L_raw);
                ASSERT(L.get_top() == 3);
                std::string name = L.get<std::string>(1);
                std::string vert = L.get<std::string>(2);
                std::string frag = L.get<std::string>(3);

                get_context().shader_holder->load(name, vert, frag);
                return 0;
            });
            m_lua_state.set_field(-2, "load_shader");

            m_lua_state.push<std::function<int(lua_State *)>>(
            [this] (lua_State *L_raw) -> int {
                lua::State L(L_raw);
                ASSERT(L.get_top() == 2);
                std::string name = L.get<std::string>(1);
                std::string fname = L.get<std::string>(2);

                get_context().sound_holder->load(name, fname);
                return 0;
            });
            m_lua_state.set_field(-2, "load_sound");

            m_lua_state.push<std::function<int(lua_State *)>>(
            [this] (lua_State *L_raw) -> int {
                lua::State L(L_raw);
                ASSERT(L.get_top() == 2);
                std::string name = L.get<std::string>(1);
                std::string fname = L.get<std::string>(2);

                get_context().music_holder->load(name, fname);
                return 0;
            });
            m_lua_state.set_field(-2, "load_music");

            m_lua_state.set_global("rts");
        }

        bool LoadingState::update(sf::Time)
        {
            m_lua_state.run_file("../res/init.lua");
            get_context().desktop->SetProperty("*", "FontName", get_context().font_holder->get_fname("font"));
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
