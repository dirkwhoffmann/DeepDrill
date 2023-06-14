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
    vector <Coord> probePoints;
    
    
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
        
private:

    //
    // Picking points
    //

    // Collect all drill locations
    void collectCoordinates(vector<dd::Coord> &remaining);

    // Picks a reference point
    ReferencePoint pickReference(const vector<Coord> &glitches);

    // Picks a collection of probe points
    void pickProbePoints(vector <Coord> &probes);


    //
    // Drilling points with the standard algorithm (for debugging)
    //

    // Drills a collection of points
    void slowDrill(const vector<Coord> &remaining);

    // Drills a single of point
    void slowDrill(const Coord &point);


    //
    // Drilling points with perturbation and series approximation
    //

    // Drills a reference point
    void drill(ReferencePoint &ref);
    
    // Drills a collection of probe points
    isize drillProbePoints(vector <Coord> &probes);

    // Drills a single probe point
    isize drillProbePoint(Coord &probe);

    // Drills a collection of delta points
    void drill(const vector<Coord> &remaining, vector<Coord> &glitchPoints);

    // Drills a single delta point
    void drill(const Coord &point, vector<Coord> &glitchPoints);
};

}
