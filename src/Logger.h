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
struct ralign { string str; ralign(const string s) : str(s) { } };

// Constants
static constexpr Endl endl;
static constexpr VSpace vspace;

}

class Logger {

    // Blank line counter
    isize blanks = 0;

public:

    Logger& operator<<(const log::Endl &arg);
    Logger& operator<<(const log::VSpace &arg);
    Logger& operator<<(const log::ralign &arg);
    Logger& operator<<(const string &arg);
    Logger& operator<<(const isize &arg);
    Logger& operator<<(const Time &arg);
};

// Default logger object
namespace log {
    extern class Logger cout;
}

}
