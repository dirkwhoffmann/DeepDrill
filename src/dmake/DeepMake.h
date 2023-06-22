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
    
    //
    // Methods from Application
    //

    const char *appName() const { return "DeepMake"; }
    const char *optstring() const;
    const option *longopts() const;
    bool isAcceptedInputFormat(Format format) const { return format == Format::INI; }
    bool isAcceptedOutputFormat(Format format) const { return format == Format::DIR; }

    void syntax() const;
    void initialize() { };
    void checkArguments();
    void run();
};

}
