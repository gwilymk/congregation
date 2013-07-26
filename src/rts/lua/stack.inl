#define LUA_STACK_NO_DEFINE

#include "stack.hpp"

#include <lua.hpp>
#include <string>
#include <map>
#include <functional>

#include "rts/common.hpp"

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

        template <>
        struct Stack<std::function<int(lua_State *)>>
        {
            typedef std::function<int(lua_State *)> lua_function;
            typedef std::map<int, lua_function> function_list;

            static int lua_function_call(lua_State *L)
            {
                lua_function *func = (lua_function *) lua_touserdata(L, lua_upvalueindex(1));
                return (*func)(L);
            }
            
            static int function_list___gc(lua_State *L)
            {
                function_list *list = (function_list *) lua_touserdata(L, 1);
                list->~map();
                return 0;
            }

            static void push(lua_State *L, const lua_function &val)
            {
                static const char key = 'k';
                lua_pushlightuserdata(L, (void *)&key);
                lua_gettable(L, LUA_REGISTRYINDEX);
                if(lua_isnil(L, -1)) {
                    lua_pop(L, 1);
                    new (lua_newuserdata(L, sizeof(function_list))) function_list();
                    luaL_newmetatable(L, "rts.function_state");
                    lua_pushcfunction(L, &Stack<lua_function>::function_list___gc);
                    lua_setfield(L, -2, "__gc");
                    lua_setmetatable(L, -2);
                    lua_pushlightuserdata(L, (void *)&key);
                    lua_pushvalue(L, -2);
                    lua_settable(L, LUA_REGISTRYINDEX);
                } 

                function_list *list = (function_list *) lua_touserdata(L, -1);
                lua_pop(L, 1);
                auto ret = list->insert(std::make_pair(list->size(), val));
                ASSERT(ret.second);

                lua_pushlightuserdata(L, &(ret.first->second));
                lua_pushcclosure(L, &lua_function_call, 1);
            }

            static lua_function get(lua_State *L, int n);
        };
    }
}
