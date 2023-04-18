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
#include "Types.h"
#include "commons.h"

namespace dd {

class Maker {
    
    // Configuration options
    struct Options &opt;

    // Name of the project
    string project;

    // Center displacement
    // PrecisionComplex shift;

    // Path to the project directory
    std::filesystem::path projectDir;

public:
    
    // Constructor
    Maker(Options &opt);
    
    // Main entry point
    void generate(); 
    
private:

    void generateProjectFile();
    void generateLocationFiles();
    void generateProfile();
    void generateMakefile();

    void writeHeader(std::ofstream &os);
    void writeDefinitions(std::ofstream &os);
    void writeTargets(std::ofstream &os);

    [[deprecated]] void writeTarget(std::ofstream &os, isize nr);
};

}
