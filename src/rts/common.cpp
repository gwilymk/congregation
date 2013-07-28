#include "common.hpp"
#include <cstdio>
#include <cstdlib>

namespace rts
{
    namespace assert
    {
        void __debug_break_here()
        {
            while(true);
        }

        void fail(const char *code, const char *file, int line)
        {
            std::fprintf(stderr, "Assertion failed (%s:%i) %s\n", file, line, code);
            __debug_break_here();
            std::exit(1);
        }
    }
}
