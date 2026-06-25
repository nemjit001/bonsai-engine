#include "Timer.hpp"

#include <chrono>

using Clock = std::chrono::steady_clock;

namespace Timer
{
    double GetTime()
    {
        auto const currentTime = Clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(currentTime).count();
    }
} // namespace Timer

