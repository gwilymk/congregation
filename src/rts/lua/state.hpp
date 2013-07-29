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
                State(lua_State *L);
                ~State();

                template <typename T> void push(const T &val);
                template <typename T> T get(int n);

                int get_top();
                void set_top(int index);
                void push_value(int index);
                void push_global(const std::string &name);
                void get_global(const std::string &name);
                void set_global(const std::string &name);
                void get_field(int index, const std::string &name);
                void set_field(int index, const std::string &name);
                void new_table();
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

            private:
                lua_State *L;
                bool m_should_cleanup;
        };
    }
}

#include "state.inl"

#endif
