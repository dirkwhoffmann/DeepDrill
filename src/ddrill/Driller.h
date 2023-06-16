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
#include "Approximator.h"
#include "Coord.h"
#include "DrillMap.h"
#include "ReferencePoint.h"
#include <random>

namespace dd {

class Driller {
    
    // Configuration options
    const struct Options &opt;
    
    // The associated drill map
    DrillMap &map;
    
    
    //
    // Perturbation parameters
    //

    // The current reference point
    ReferencePoint ref;

    
    //
    // Series approximation parameters
    //
    
    // Computed coefficients
    Approximator approximator = Approximator(opt);
    
    // The probe points
    std::vector<Coord> probePoints;
    
    
    //
    // Initialization
    //

public:

    Driller(const Options &options, DrillMap &map);
    
    
    //
    // Computing the Mandelbrot set
    //
                
public:
    
    // Computes the drill map (main entry point)
    void drill();
        

    //
    // Picking points
    //

private:

    // Collect all drill locations
    void collectCoordinates(std::vector<dd::Coord> &remaining);

    // Picks a reference point
    ReferencePoint pickReference(const std::vector<Coord> &glitches);

    // Picks a collection of probe points
    void pickProbePoints(std::vector<Coord> &probes);


    //
    // Drilling points with perturbation and series approximation
    //

private:
    
    // Drills a reference point
    void drill(ReferencePoint &ref);
    
    // Drills a collection of probe points
    isize drillProbePoints(std::vector<Coord> &probes);

    // Drills a single probe point
    isize drillProbePoint(Coord &probe);

    // Drills a collection of delta points
    void drill(const std::vector<Coord> &remaining, std::vector<Coord> &glitchPoints);

    // Drills a single delta point
    void drill(const Coord &point, std::vector<Coord> &glitchPoints);
};

}
