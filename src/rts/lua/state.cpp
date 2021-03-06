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

#include "state.hpp"

#include <lua.hpp>
#include <cstdio>
#include <string>
#include <stdexcept>

namespace rts
{
    namespace lua
    {
        State::State():
            m_should_cleanup(true)
        {
            L = luaL_newstate();
            luaL_openlibs(L);
        }

        State::State(lua_State *L):
            L(L),
            m_should_cleanup(false)
        {
        }

        State::~State()
        {
            if(m_should_cleanup)
                lua_close(L);
        }

        int State::get_top()
        {
            return lua_gettop(L);
        }

        void State::set_top(int index)
        {
            lua_settop(L, index);
        }

        void State::push_value(int index)
        {
            lua_pushvalue(L, index);
        }

        void State::push_global(const std::string &name)
        {
            lua_getglobal(L, name.c_str());
        }

        void State::get_global(const std::string &name)
        {
            lua_getglobal(L, name.c_str()); 
        }

        void State::set_global(const std::string &name)
        {
            lua_setglobal(L, name.c_str());
        }

        void State::get_field(int index, const std::string &name)
        {
            lua_getfield(L, index, name.c_str()); 
        }

        void State::set_field(int index, const std::string &name)
        {
            lua_setfield(L, index, name.c_str());
        }

        void State::new_table()
        {
            lua_newtable(L); 
        }

        void State::remove(int index)
        {
            lua_remove(L, index); 
        }

        int State::type(int index)
        {
            return lua_type(L, index); 
        }

        const char *State::type_name(int index)
        {
            return lua_typename(L, index);
        }

        void State::load_file(const std::string &filename)
        {
            if(luaL_loadfile(L, filename.c_str()))
                throw std::runtime_error("Failed to load file: " + filename);
        }

        void State::run_file(const std::string &filename)
        {
            load_file(filename);
            call(0);
        }

        void State::call(int nresults)
        {
            call_intern(nresults, 0);
        }

        void State::call_intern(int nresults, int nargs)
        {
            if(lua_pcall(L, nargs, nresults, 0))
                throw std::runtime_error("Lua error: " + get<std::string>(-1));
        }

        void State::stack_dump()
        {
            int top = get_top();

            std::printf("L U A   S T A C K   D U M P\n");
            std::printf("===========================\n\n");

            for(int i = 1; i <= top; ++i) {
                int t = type(i);

                switch(t) {
                    case LUA_TSTRING:
                        std::printf("%i - %s\n", i, get<std::string>(i).c_str());
                        break;
                    case LUA_TBOOLEAN:
                        std::printf("%i - %s\n", i, get<bool>(i) ? "true" : "false");
                        break;
                    case LUA_TNUMBER:
                        std::printf("%i - %g\n", i, get<double>(i));
                        break;
                    default:
                        std::printf("%i - %s\n", i, type_name(t));
                        break;
                }
            }
        }
    }
}
