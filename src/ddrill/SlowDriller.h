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
#include "Coord.h"
#include "DrillMap.h"

namespace dd {

class SlowDriller {

    // Configuration options
    const struct Options &opt;

    // The associated drill map
    DrillMap &map;

    
    //
    // Initialization
    //

public:

    SlowDriller(const Options &o, DrillMap &m) : opt(o), map(m) { }


    //
    // Computing the Mandelbrot set
    //

public:

    // Computes the drill map (main entry point)
    void drill();

private:

    // Drills a collection of points
    void drill(const std::vector<Coord> &remaining);

    // Drills a single of point
    void drill(const Coord &point);
};

}
