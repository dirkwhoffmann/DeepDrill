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

#include "commons.h"

namespace dd {

namespace log {

// Wrapper structures
struct Endl { };
struct VSpace { };
struct Flush { };
struct ralign { string str; ralign(const string s) : str(s) { } };
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

    //
    std::ostream &stream;

    // Blank line counter
    isize blanks = 0;
    
    // Indicates if all output should be omitted
    bool silent = false;

    // Color index
    isize color = 0;

    // Bold flag
    bool bold = false;

public:

    Logger(std::ostream &stream) : stream(stream) { };

    void setSilent(bool value) { silent = value; }
    
    Logger& operator<<(const log::Endl &arg);
    Logger& operator<<(const log::VSpace &arg);
    Logger& operator<<(const log::Flush &arg);
    Logger& operator<<(const log::ralign &arg);
    Logger& operator<<(const log::Black &arg) { setColor(0, bold); return *this; }
    Logger& operator<<(const log::Red &arg) { setColor(1, bold); return *this; }
    Logger& operator<<(const log::Green &arg) { setColor(2, bold); return *this; }
    Logger& operator<<(const log::Yellow &arg) { setColor(3, bold); return *this; }
    Logger& operator<<(const log::Blue &arg) { setColor(4, bold); return *this; }
    Logger& operator<<(const log::Purple &arg) { setColor(5, bold); return *this; }
    Logger& operator<<(const log::Cyan &arg) { setColor(6, bold); return *this; }
    Logger& operator<<(const log::White &arg) { setColor(7, bold); return *this; }
    Logger& operator<<(const log::Bold &arg) { setColor(color, 1); return *this; }
    Logger& operator<<(const log::Light &arg) { setColor(color, 0); return *this; }
    Logger& operator<<(const string &arg);
    Logger& operator<<(const isize &arg);
    Logger& operator<<(const Time &arg);
    Logger& operator<<(const struct Coord &arg);
    Logger& operator<<(const StandardComplex& arg);
    Logger& operator<<(const ExtendedDouble& arg);
    Logger& operator<<(const ExtendedComplex& arg);
    Logger& operator<<(const PrecisionComplex& arg);

    void setColor(isize c, bool b);
};

// Default logger (writes to stdout)
namespace log { extern class Logger cout; }

}
