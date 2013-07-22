#ifndef COMMON_HPP
#define COMMON_HPP

#ifndef NDEBUG
#define assert(x) \
    do { if(!(x)) { rts::assert::fail(#x, __FILE__, __LINE__); } } while(__LINE__ == -1)
#else
#define assert(x) \
    do { (void)sizeof(x); } while(__LINE__ == -1)
#endif

namespace rts
{
    namespace assert
    {
        void fail(const char *code, const char *file, int line);
    }
}

#endif
