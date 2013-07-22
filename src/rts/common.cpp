#include "common.hpp"
#include <cstdio>
#include <cstdlib>

namespace rts
{
    namespace assert
    {
        void fail(const char *code, const char *file, int line)
        {
            std::printf("Assertion failed (%s:%i) %s\n", file, line, code);
            std::exit(1);
        }
    }
}
