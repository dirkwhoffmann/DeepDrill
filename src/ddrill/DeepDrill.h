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
#include "Colorizer.h"

namespace dd {

class DeepDrill : public Application {

    Colorizer colorizer = Colorizer(opt);


    //
    // Methods from Application
    //

    const char *appName() { return "DeepDrill"; }
    void syntax();
    void initialize() { };
    void parseArguments(int argc, char *argv[]);
    void checkCustomArguments();
    void run();
};

}
