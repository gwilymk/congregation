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

#include "common.hpp"
#include <cstdio>
#include <cstdlib>

namespace rts
{
    namespace assert
    {
        void __debug_break_here()
        {
#ifndef NDEBUG
            while(true); // so that the program doesn't stop running
#endif
        }

        void fail(const char *code, const char *file, int line)
        {
            std::fprintf(stderr, "Assertion failed (%s:%i) %s\n", file, line, code);
            __debug_break_here();
            std::exit(1);
        }
    }
}
