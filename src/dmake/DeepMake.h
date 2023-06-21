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
#include "Application.h"

namespace dd {

class DeepMake : public Application {

    isize maxKeyframes = LONG_MAX;

    
    //
    // Methods from Application
    //

    const char *appName() { return "DeepMake"; }
    const char *optstring();
    const option *longopts();
    bool isAcceptedInputFormat(Format format) const { return format == Format::INI; }
    bool isAcceptedOutputFormat(Format format) const { return format == Format::DIR; }

    void syntax();
    void initialize() { };
    void checkArguments();
    void run();
};

}
