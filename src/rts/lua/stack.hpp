#ifndef LUASTACK_HPP
#define LUASTACK_HPP

struct lua_State;

#include <string>

namespace rts
{
    namespace lua
    {
        template <typename T> 
        struct Stack
        {
            static void push(lua_State *L, const T &val);
            static T get(lua_State *L, int n);
        };
    }
}

#include "stack.inl"

#endif
