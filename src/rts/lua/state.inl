#include "rts/lua/stack.hpp"

namespace rts
{
    namespace lua
    {
        template <typename T>
        void State::push(const T &val)
        {
            Stack<T>::push(L, val);
        }

        template <typename T> 
        T State::get(int n)
        {
            return Stack<T>::get(L, n);
        }

        template <typename T, typename... Args> void State::call_intern(int nresults, int nargs, T value, Args... args)
        {
            push(value);
            call_intern(nresults, nargs, args...);
        }
        
        template <typename T, typename... Args> void State::call(int nresults, T value, Args... args)
        {
            call_intern(nresults, sizeof...(Args) + 1, value, args...); 
        }
    }
}
