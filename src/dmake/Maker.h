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
#include "gmpxx.h"

namespace dd {

class Maker {

    // Refeference to the application
    class Application &app;

    // Configuration options
    struct Options &opt;

    // Path to the project directory
    fs::path projectDir;

    // File tracker
    enum class Action { CREATED, SKIPPED, MODIFIED };
    std::vector <std::pair<fs::path, Action>> report;

public:
    
    // Constructor
    Maker(Application &app, Options &opt);
    
    // Main entry point
    void generate(); 
    
private:

    // Generators
    void generateProjectFile();
    void generateIniFiles();
    void generateIniFile(isize nr, const mpf_class &zoom);
    void generateMakefile();

    void writeLocationSection(std::ofstream &os);
    void writeMapSection(std::ofstream &os);
    void writeImageSection(std::ofstream &os);
    void writePaletteSection(std::ofstream &os);
    void writeTextureSection(std::ofstream &os);
    void writeLightingSection(std::ofstream &os);
    void writeVideoSection(std::ofstream &os);
    void writePerturbationSection(std::ofstream &os);
    void writeApproximationSection(std::ofstream &os);
    void writeAreacheckSection(std::ofstream &os);
    void writePeriodcheckSection(std::ofstream &os);
    void writeAttractorcheckSection(std::ofstream &os);

    void writeHeader(std::ofstream &os);
    void writeDefinitions(std::ofstream &os);
    void writeTargets(std::ofstream &os);

    void copy(const fs::path &from, const fs::path &to);
};

}
