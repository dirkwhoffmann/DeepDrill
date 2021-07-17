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

// Constants
static constexpr Endl endl;
static constexpr VSpace vspace;
static constexpr Flush flush;

}

class Logger {

    // Blank line counter
    isize blanks = 0;

public:

    Logger& operator<<(const log::Endl &arg);
    Logger& operator<<(const log::VSpace &arg);
    Logger& operator<<(const log::Flush &arg);
    Logger& operator<<(const log::ralign &arg);
    Logger& operator<<(const string &arg);
    Logger& operator<<(const isize &arg);
    Logger& operator<<(const Time &arg);
    Logger& operator<<(const class Coord &arg);
    Logger& operator<<(const StandardComplex& arg);
    Logger& operator<<(const ExtendedDouble& arg);
    Logger& operator<<(const ExtendedComplex& arg);
    Logger& operator<<(const PrecisionComplex& arg);
};

// Default logger object
namespace log {
    extern class Logger cout;
}

}
