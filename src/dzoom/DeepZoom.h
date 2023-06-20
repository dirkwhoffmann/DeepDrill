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

class DeepZoom : public Application {

    //
    // Methods from Application
    //

    const char *appName() { return "DeepZoom"; }
    void syntax();
    void initialize();
    bool isAcceptedInputFormat(Format format) const;
    bool isAcceptedOutputFormat(Format format) const;
    void parseArguments(int argc, char *argv[]);
    void checkArguments();
    void run();
};

}
