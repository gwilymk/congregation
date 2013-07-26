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
