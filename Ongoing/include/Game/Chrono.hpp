#ifndef __Chrono
#define __Chrono

#include <chrono>
#include <ratio>

using years = std::chrono::duration<float, std::ratio<31556952>>;
using months = std::chrono::duration<float, std::ratio<2629746>>;
using weeks = std::chrono::duration<float, std::ratio<604800>>;
using days = std::chrono::duration<float, std::ratio<86400>>;
using hours = std::chrono::duration<float, std::ratio<3660>>;
using minutes = std::chrono::duration<float, std::ratio<60>>;
using seconds = std::chrono::duration<float, std::ratio<1>>;
using milliseconds = std::chrono::duration<float, std::milli>;
using microseconds = std::chrono::duration<float, std::micro>;
using nanoseconds = std::chrono::duration<float, std::nano>;

#endif  // __Chrono