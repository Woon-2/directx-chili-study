#ifndef __Invocable
#define __Invocable

#include <utility>

template < class ... Ts >
consteval bool is_invocation_valid(Ts&& ...)  { return false; }

template < class Lambda, class ... Args >
consteval auto is_invocation_valid(Lambda lambda, Args&& ... args) 
    -> decltype( lambda( std::declval<Args>()... ), bool{} ) { return true; }

#endif  // __Invocable