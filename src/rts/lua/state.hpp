#ifndef LUASTATE_HPP
#define LUASTATE_HPP

struct lua_State;

#include <SFML/System/NonCopyable.hpp>
#include <string>

namespace rts
{
    namespace lua
    {
        class State : private sf::NonCopyable
        {
            public:
                State();
                ~State();

                template <typename T> void push(const T &val);
                template <typename T> T get(int n);

                int get_top();
                void set_top(int index);
                void push_value(int index);
                void push_global(const std::string &name);
                void remove(int index);
                int type(int index);
                const char *type_name(int index);

                void call(int nresults);
                template <typename T, typename... Args> void call(int nresults, T value, Args... args);

                void load_file(const std::string &filename);
                void run_file(const std::string &filename);

                void stack_dump();

            private:
                void call_intern(int nresults, int nargs);
                template <typename T, typename... Args> void call_intern(int nresults, int nargs, T value, Args... args);

                lua_State *L;
        };
    }
}

#include "state.inl"

#endif
