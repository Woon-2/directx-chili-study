#ifndef __Timer
#define __Timer

#include <chrono>
#include <ratio>

template <class Rep, class Period = std::milli>
class Timer {
public:
    using time_point = std::chrono::steady_clock::time_point;
    using clock = std::chrono::steady_clock;
    using duration = std::chrono::duration<Rep, Period>;
    using rep = Rep;
    using period = Period;

    Timer() noexcept {
        last_ = clock::now();
    }

    [[maybe_unused]] Rep mark() noexcept {
        const auto old = last_;
        last_ = clock::now();
        const auto elapsed
            = std::chrono::duration_cast<duration>(last_ - old);
        return elapsed.count();
    }

    Rep peek() const noexcept {
        return std::chrono::duration_cast<duration>(
            clock::now() - last_
        ).count();
    }

private:
    time_point last_;
};

#endif  // __Timer