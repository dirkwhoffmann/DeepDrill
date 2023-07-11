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
#include "DrillMap.h"

namespace dd {

class MapAnalyzer {

    isize width;
    isize height;
    isize total;
    
    struct {

        isize total = 0;
        isize interior = 0;
        isize exterior = 0;
        isize unprocessed = 0;
        isize glitches = 0;

    } spots;

    struct {

        isize total = 0;
        isize bulb = 0;
        isize cartioid = 0;
        isize approximations = 0;
        isize periods = 0;
        isize attractors = 0;

    } optspots;

    struct {

        isize total = 0;
        isize interior = 0;
        isize exterior = 0;

    } iterations;

    struct {

        isize total = 0;
        isize bulb = 0;
        isize cartioid = 0;
        isize approximations = 0;
        isize periods = 0;
        isize attractors = 0;

    } saved;


    //
    // Initializing
    //

public:

    MapAnalyzer() { };
    MapAnalyzer(const DrillMap &map) { analyze(map); }


    //
    // Analyzing
    //

public:

    // Analyzes a drill map
    void analyze(const DrillMap &map);

    // Prints a summary report
    void print();
};

}
