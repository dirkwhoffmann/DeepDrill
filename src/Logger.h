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

class Logger {

    // Blank line counter
    isize blanks = 0;

public:

    // Wrapper structures
    struct Endl { };
    struct VSpace { };
    
    // static constexpr Endl endl;
    // static constexpr VSpace vspace;

    Logger& operator<<(const string &arg);
    Logger& operator<<(const isize &arg);
    Logger& operator<<(const Endl &arg);
    Logger& operator<<(const VSpace &arg);
};

namespace log {

    static constexpr Logger::Endl endl;
    static constexpr Logger::VSpace vspace;
}

}
