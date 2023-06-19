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
#include "IO.h"

namespace dd {

class Maker {

    // Refeference to the application
    class Application &app;

    // Configuration options
    struct Options &opt;

    // Name of the project
    string project;

    // Path to the project directory
    fs::path projectDir;

public:
    
    // Constructor
    Maker(Application &app, Options &opt);
    
    // Main entry point
    void generate(); 
    
private:

    void generateProjectFile(std::vector <string> &skipped);
    void generateLocationFiles(std::vector <string> &skipped);
    // void generateProfile(std::vector <string> &skipped);
    void generateMakefile(std::vector <string> &skipped);

    void writeLocationSection(std::ofstream &os);
    void writeMapSection(std::ofstream &os);
    void writeImageSection(std::ofstream &os);
    void writePreviewImageSection(std::ofstream &os);
    void writeColorsSection(std::ofstream &os);
    void writeVideoSection(std::ofstream &os);
    void writePerturbationSection(std::ofstream &os);
    void writeApproximationSection(std::ofstream &os);
    void writeOverrideSection(std::ofstream &os);

    void writeHeader(std::ofstream &os);
    void writeDefinitions(std::ofstream &os);
    void writeTargets(std::ofstream &os);
};

}
