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

    // File tracker
    enum class Action { CREATED, SKIPPED, MODIFIED };
    std::vector <std::pair<fs::path, Action>> report;

public:
    
    // Constructor
    Maker(Application &app, Options &opt);
    
    // Main entry point
    void generate(); 
    
private:

    // Filenames
    fs::path iniFile() { return "deepzoom.ini"; }
    fs::path movFile() { return "deepzoom.mov"; }
    fs::path rawFile(isize i) { return "keyframe_" + std::to_string(i); }
    fs::path iniFile(isize i) { return rawFile(i).string() + ".ini"; }
    fs::path mapFile(isize i) { return rawFile(i).string() + ".map"; }
    fs::path imgFile(isize i) { return rawFile(i).string() + ".jpg"; }

    // Generators
    void generateProjectFile();
    void generateIniFiles();
    void generateIniFile(isize nr);
    void generateMakefile();

    void writeLocationSection(std::ofstream &os);
    void writeMapSection(std::ofstream &os);
    void writeImageSection(std::ofstream &os);
    void writeColorsSection(std::ofstream &os);
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
