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

#pragma once

#include "config.h"
#include "Types.h"
#include <ctime>

namespace dd {

class Time {
    
public:
    
    i64 ticks = 0;

public:
    
    static Time now();
    
    static Time microseconds(i64 value) { return Time(value * 1000); }
    static Time milliseconds(i64 value)  { return Time(value * 1000000); }
    static Time seconds(i64 value) { return Time(value * 1000000000); }
    static Time seconds(float value) { return Time(i64(value * 1000000000.f)); }
    static std::tm local(const std::time_t &time);

    Time() { };
    Time(i64 value) : ticks(value) { };

    i64 asNanoseconds()  const { return ticks; }
    i64 asMicroseconds() const { return ticks / 1000; }
    i64 asMilliseconds() const { return ticks / 1000000; }
    float asSeconds()    const { return ticks / 1000000000.f; }
    string asString() const;
    
    bool operator==(const Time &rhs) const;
    bool operator!=(const Time &rhs) const;
    bool operator<=(const Time &rhs) const;
    bool operator>=(const Time &rhs) const;
    bool operator<(const Time &rhs) const;
    bool operator>(const Time &rhs) const;
    Time operator+(const Time &rhs) const;
    Time operator-(const Time &rhs) const;
    Time operator*(const int i) const;
    Time& operator+=(const Time &rhs);
    Time& operator-=(const Time &rhs);
    Time& operator*=(const int i);
    Time abs() const;
    Time diff() const;
    
    void sleep();
    void sleepUntil();
    
    friend std::ostream& operator<<(std::ostream& os, const Time& value);
};

class Clock {
        
    Time start;
    Time elapsed;
    
    bool paused = false;

public:
    
    Clock();

    Time getElapsedTime();
    
    bool isPaused() const { return paused; }
    bool isRunning() const { return !paused; }
    
    Time stop() { return set(true); }
    Time go() { return set(false); }
    Time reset() { return set(true, Time::now(), 0); }
    Time restart() { return set(false, Time::now(), 0); }

private:

    Time updateElapsed();
    Time set(bool newPaused);
    Time set(bool newPaused, Time newStart, Time newElapsed);
};

}
