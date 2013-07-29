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

#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <sstream>

#define ASSERT(x) \
    do { if(!(x)) { rts::assert::fail(#x, __FILE__, __LINE__); } } while(__LINE__ == -1)

namespace rts
{
    const int VERSION = 1;

    namespace assert
    {
        void fail(const char *code, const char *file, int line);
    }

    template <typename T>
    std::string number_to_string(T Number)
    {
        std::ostringstream ss;
        ss << Number;
        return ss.str();
    }

}

#endif
