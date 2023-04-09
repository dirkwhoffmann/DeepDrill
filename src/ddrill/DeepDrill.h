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

class DeepDrill {

    // Extracted command line arguments
    std::vector <string> profiles;
    std::vector <string> inputs;
    std::vector <string> outputs;

public:

    // Main entry point
    void main(int argc, char *argv[]);

private:
    
    // Helper methods for parsing command line arguments
    void parseArguments(int argc, char *argv[], map<string,string> &keys);
    void checkArguments(map<string,string> &keys);
    void readInputs(map<string,string> &keys);
    void readOutputs(map<string,string> &keys);
    void readProfiles(map<string,string> &keys);
    
    // Executes the drill pipeline or the Makefile generator
    void runPipeline(struct Options &opt);
    void runMaker(struct Options &opt);

    // Prints a progress line in batch mode
    void printProgress(Options &opt);

    // Sets up the GMP library
    void setupGmp(std::map <string,string> &keys);
};

}
