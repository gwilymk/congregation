#define LUA_STACK_NO_DEFINE

#include "stack.hpp"

#include <lua.hpp>
#include <string>

namespace rts
{
    namespace lua
    {
        template <>
        struct Stack<int> 
        {
            static void push(lua_State *L, const int &val)
            {
                lua_pushinteger(L, val);           
            }

            static int get(lua_State *L, int n)
            {
                return luaL_checkinteger(L, n);
            }
        };

        template <>
        struct Stack<double> 
        {
            static void push(lua_State *L, const double &val)
            {
                lua_pushnumber(L, val);
            }

            static double get(lua_State *L, int n)
            {
                return luaL_checknumber(L, n);
            }
        };

        template <>
        struct Stack<std::string> 
        {
            static void push(lua_State *L, const std::string &val)
            {
                lua_pushstring(L, val.c_str());
            }

            static std::string get(lua_State *L, int n)
            {
                return std::string(luaL_checkstring(L, n));
            }
        };

        template <>
        struct Stack<bool>
        {
            static void push(lua_State *L, const bool &val)
            {
                lua_pushboolean(L, val);
            }

            static bool get(lua_State *L, int n)
            {
                return lua_toboolean(L, n);
            }
        };
    }
}
