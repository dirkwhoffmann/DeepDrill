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
#include "Colorizer.h"

namespace dd {

class DeepDrill : public Application {

    // The drill map
    DrillMap drillMap = DrillMap(opt);

    // Colorizer for converting the drill map into an image
    Colorizer colorizer = Colorizer(opt);


    //
    // Methods from Application
    //

public:

    void run();

private:

    const char *appName() { return "DeepDrill"; }
    void syntax();
    void initialize() { };
    bool isAcceptedInputFormat(Format format) const;
    bool isAcceptedOutputFormat(Format format) const;
    void parseArguments(int argc, char *argv[]);
    void checkArguments();


    //
    // Auxiliary methods
    //

    void runDriller();
    void generateOutputs();
};

}
