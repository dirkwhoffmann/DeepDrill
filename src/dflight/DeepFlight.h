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
#include "Options.h"

namespace dd {

class DeepFlight {

    // Extracted command line arguments
    std::vector <string> profiles;
    std::vector <string> inputs;
    std::vector <string> outputs;

    // Config options
    Options opt;

    // Paths to all image files
    std::vector <string> imageFiles;

public:

    // Main entry point
    void main(int argc, char *argv[]);

private:

    // Helper methods for parsing command line arguments
    void parseArguments(int argc, char *argv[]);
    void checkArguments();
    void readInputs();
    void readOutputs();
    void readProfiles();

    // Sets up the GMP library
    void setupGmp(isize accuracy);
};

}
