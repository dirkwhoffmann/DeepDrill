// -----------------------------------------------------------------------------
// This file is part of DeepDrill
//
// A Mandelbrot generator based on perturbation and series approximation
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "Chrono.h"
#include "IO.h"
#include <sstream>

#ifdef __MACH__
#include <mach/mach_time.h>
#endif

namespace dd {

#ifdef __MACH__

//
// macOS
//

static struct mach_timebase_info timebaseInfo()
{
    struct mach_timebase_info tb;
    mach_timebase_info(&tb);
    return tb;
}

Time
Time::now()
{
    static struct mach_timebase_info tb = timebaseInfo();
    return (i64)mach_absolute_time() * tb.numer / tb.denom;
}

void
Time::sleep()
{
    static struct mach_timebase_info tb = timebaseInfo();
    if (ticks > 0) {
        mach_wait_until(now().asNanoseconds() + (ticks * tb.denom / tb.numer));
    }
}

void
Time::sleepUntil()
{
    static struct mach_timebase_info tb = timebaseInfo();
    mach_wait_until(ticks * tb.denom / tb.numer);
}

#else
    
//
// Linux
//

Time
Time::now()
{
    struct timespec ts;
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    return (i64)ts.tv_sec * 1000000000 + ts.tv_nsec;
}

void
Time::sleep()
{
    struct timespec req, rem;
    
    if (ticks > 0) {
        req.tv_sec = 0;
        req.tv_nsec = ticks;
        nanosleep(&req, &rem);
    }
}

void
Time::sleepUntil()
{
    (*this - now()).sleep();
}

#endif

//
// All platforms
//

std::ostream& operator<<(std::ostream& os, const Time& value)
{
    auto usec = value.asMicroseconds();

    auto msec = usec / (1000LL);
    auto hsec = usec / (1000LL * 10LL);
    auto sec  = usec / (1000LL * 1000LL);
    auto min  = sec  / (1000LL * 1000LL * 60LL);
    auto hrs  = min  / (1000LL * 1000LL * 60LL * 60LL);

    msec %= 1000;
    hsec %= 100;
    sec  %= 60;
    min  %= 60;

    os << std::setfill('0');

    if (hrs) {

        os << std::setw(0) << hrs << ":";
        os << std::setw(2) << min << ":";
        os << std::setw(2) << sec << " hrs";

    } else if (min) {

        os << std::setw(0) << min << ":";
        os << std::setw(2) << sec << " min";

    } else {

        os << std::setw(0) << sec << ".";
        os << std::setw(2) << hsec << " sec";
    }

    return os;
}

string
Time::asString() const
{
    auto ss = (isize)asSeconds();
    auto mm = ss / 60;
    auto hh = mm / 60;

    std::stringstream stream;
    
    stream << std::setfill('0');
    stream << std::setw(2) << hh << ":";
    stream << std::setw(2) << mm % 60 << ":";
    stream << std::setw(2) << ss % 60;
    stream << std::setfill(' ');

    return stream.str();
}

bool
Time::operator==(const Time &rhs) const
{
    return this->asNanoseconds() == rhs.asNanoseconds();
}

bool
Time::operator!=(const Time &rhs) const
{
    return this->asNanoseconds() == rhs.asNanoseconds();
}

bool
Time::operator<=(const Time &rhs) const
{
    return this->asNanoseconds() <= rhs.asNanoseconds();
}

bool
Time::operator>=(const Time &rhs) const
{
    return this->asNanoseconds() >= rhs.asNanoseconds();
}

bool
Time::operator<(const Time &rhs) const
{
    return this->asNanoseconds() < rhs.asNanoseconds();
}

bool
Time::operator>(const Time &rhs) const
{
    return this->asNanoseconds() > rhs.asNanoseconds();
}

Time
Time::operator+(const Time &rhs) const
{
    return Time(this->ticks + rhs.ticks);
}

Time
Time::operator-(const Time &rhs) const
{
    return Time(this->ticks - rhs.ticks);
}

Time
Time::operator*(const int i) const
{
    return Time(i * this->ticks);
}

Time&
Time::operator+=(const Time &rhs)
{
    return *this = *this + rhs;
}

Time&
Time::operator-=(const Time &rhs)
{
    return *this = *this - rhs;
}

Time&
Time::operator*=(const int i)
{
    return *this = *this * i;
}

Time
Time::abs() const
{
    return Time(ticks >= 0 ? ticks : -ticks);
}

Time
Time::diff() const {
    return *this - now();
}

Clock::Clock()
{
    start = Time::now();
}

void
Clock::updateElapsed()
{
    auto now = Time::now();
    if (!paused) elapsed += now - start;
    start = now;
}

Time
Clock::getElapsedTime()
{
    updateElapsed();
    return elapsed;
}

Time
Clock::stop()
{
    updateElapsed();
    paused = true;
    return elapsed;
}

Time
Clock::go()
{
    updateElapsed();
    paused = false;
    return elapsed;
}

Time
Clock::restart()
{
    updateElapsed();
    auto result = elapsed;

    start = Time::now();
    elapsed = 0;
    paused = false;
    
    return result;
}

}
