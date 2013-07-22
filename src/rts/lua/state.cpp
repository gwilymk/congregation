#include "state.hpp"

#include <lua.hpp>
#include <cstdio>
#include <string>
#include <stdexcept>

namespace rts
{
    namespace lua
    {
        State::State()
        {
            L = luaL_newstate();
            luaL_openlibs(L);
        }

        State::~State()
        {
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

            std::printf("L U A   S T A C K   D U M P");
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
                        std::printf("%i - %s\n", i, type_name(i));
                        break;
                }
            }
        }
    }
}
