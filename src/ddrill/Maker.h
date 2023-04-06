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

class Maker {
    
    // Configuration options
    struct DrillOptions &opt;

    // Name of the project
    string project;

    // Path to the project directory
    std::filesystem::path projectDir;

public:
    
    // Constructor
    Maker(DrillOptions &opt);
    
    // Main entry point
    void generate(); 
    
private:

    void generateLocationFiles(isize count);
    void generateLocationFile(isize nr);
    void generateProfile();
    void generateMakefile(isize numImages);

    void writeHeader(std::ofstream &os);
    void writeTarget(std::ofstream &os, isize nr);
};

}
