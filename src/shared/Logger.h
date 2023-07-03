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
#include "Chrono.h"
#include "Dynamic.h"
#include "Exception.h"
#include "StandardComplex.h"
#include "ExtendedComplex.h"
#include "PrecisionComplex.h"

#include <thread>

namespace dd {

namespace log {

// Wrapper structures
struct Endl { };
struct VSpace { };
struct Flush { };
struct ralign { string s; int w; ralign(const string s, int w = 32) : s(s), w(w) { } };
struct Normal { };
struct Black { };
struct Red { };
struct Green { };
struct Yellow { };
struct Blue { };
struct Purple { };
struct Cyan { };
struct White { };
struct Bold { };
struct Light { };

// Constants
static constexpr Endl endl;
static constexpr VSpace vspace;
static constexpr Flush flush;
static constexpr Normal normal;
static constexpr Black black;
static constexpr Red red;
static constexpr Green green;
static constexpr Yellow yellow;
static constexpr Blue blue;
static constexpr Purple purple;
static constexpr Cyan cyan;
static constexpr White white;
static constexpr Bold bold;
static constexpr Light light;

}

class Logger {

    friend class Silence;

    // The underlying output stream
    std::ostream &stream;

    // Limits the output to a particular thread
    std::optional<std::thread::id> id;
    
    // Blank line counter
    isize blanks = 0;
    
    // Indicates if all output should be omitted
    int muted = 0;

public:

    Logger(std::ostream &stream) : stream(stream) { };

    void restrict(std::thread::id tid = std::this_thread::get_id()) { id = tid; }

    void mute() { muted++; }
    void unmute() { muted--; }

    Logger& operator<<(const log::Endl &arg);
    Logger& operator<<(const log::VSpace &arg);
    Logger& operator<<(const log::Flush &arg);
    Logger& operator<<(const log::ralign &arg);
    Logger& operator<<(const log::Normal &arg) { *this << "\033[m"; return *this; }
    Logger& operator<<(const log::Black &arg) { *this << "\033[30m"; return *this; }
    Logger& operator<<(const log::Red &arg) { *this << "\033[31m"; return *this; }
    Logger& operator<<(const log::Green &arg) { *this << "\033[32m"; return *this; }
    Logger& operator<<(const log::Yellow &arg) { *this << "\033[33m"; return *this; }
    Logger& operator<<(const log::Blue &arg) { *this << "\033[34m"; return *this; }
    Logger& operator<<(const log::Purple &arg) { *this << "\033[35m"; return *this; }
    Logger& operator<<(const log::Cyan &arg) { *this << "\033[36m"; return *this; }
    Logger& operator<<(const log::White &arg) { *this << "\033[37m"; return *this; }
    Logger& operator<<(const log::Bold &arg) {  *this << "\033[1m"; return *this; }
    Logger& operator<<(const log::Light &arg) { *this << "\033[0m"; return *this; }
    Logger& operator<<(const string &arg);
    Logger& operator<<(const fs::path &arg);
    Logger& operator<<(const char &arg);
    Logger& operator<<(const char *arg);
    Logger& operator<<(const bool &arg);
    Logger& operator<<(const isize &arg);
    Logger& operator<<(const usize &arg);
    Logger& operator<<(const double &arg);
    Logger& operator<<(const Time &arg);
    Logger& operator<<(const struct Coord &arg);
    Logger& operator<<(const StandardComplex& arg);
    Logger& operator<<(const ExtendedDouble& arg);
    Logger& operator<<(const ExtendedComplex& arg);
    Logger& operator<<(const PrecisionComplex& arg);
    Logger& operator<<(const Exception& arg);
    Logger& operator<<(const Dynamic<float>& arg);

private:

    bool verbose() const;

};

// Default logger (writes to stdout)
namespace log { extern class Logger cout; }

}
