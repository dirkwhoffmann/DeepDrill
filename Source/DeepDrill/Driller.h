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
#include "MathTypes.h"
#include "Coefficients.h"
#include "Coord.h"
#include "DrillMap.h"
#include "Options.h"
#include "Palette.h"
#include "ReferencePoint.h"
#include <random>

namespace dd {

class Driller {

    friend class Application;
    
    // Configuration options
    const Options &opt;
    
    // Stop flag for interrupting the computation
    bool stop = false;

    // The part of the Mandelbrot set to be computed
    DrillMap map = DrillMap(opt);
        
    // The color palette
    Palette palette = Palette(opt);

    
    //
    // Perturbation parameters
    //

    // The current reference point
    ReferencePoint ref;

    // Pixel tolerance (fraction of pixels that can be glitched)
    double badPixels = 0.001;

    
    //
    // Series approximation parameters
    //
    
    // Computed coefficients
    Coefficients coeff;
    
    // The probe points
    vector <Coord> probePoints;
    
    
    //
    // Initialization
    //

public:

    Driller(const Options &options);
    
    
    //
    // Computing the Mandelbrot set
    //
    
public:
    
    // Computes the Mandelbrot image (main entry point)
    void drill();
            
private:
    
    // Drills a collection of points or a single point
    void drill(const vector<Coord> &remaining, vector<Coord> &glitchPoints);
    void drill(const Coord &point, vector<Coord> &glitchPoints);
    
    // Picks a reference point
    ReferencePoint pickReference(const vector<Coord> &glitches);


    //
    // Series approximation
    //
    
    // Picks the probe points
    void pickProbePoints(vector <Coord> &probes);

    // Drills the probe points
    isize drillProbePoints(vector <Coord> &probes);
    isize drillProbePoint(Coord &probe);
};

}
