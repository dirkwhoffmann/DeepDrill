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
#include "DrillMap.h"
#include "ColorMap.h"

namespace dd {

class DeepDrill : public Application {

    // The drill map
    DrillMap drillMap = DrillMap(opt);

    // A colorizer for converting the drill map into an image
    ColorMap colorMap = ColorMap(opt);


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
