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
#include "Coord.h"

namespace dd {

struct ReferenceIteration {
        
    // zn in standard precision format
    StandardComplex standard;
    
    // zn in extended precision format
    ExtendedComplex extended;
    
    // 2*zn in extended precision format
    ExtendedComplex extended2;
    
    // Glitch tolerance for this iteration
    double tolerance;
        
    
    //
    // Initializing
    //
    
    ReferenceIteration(PrecisionComplex c, double tolerance);
};

struct ReferencePoint {
    
    // The pixel coordinate of this point
    Coord coord;
    
    // The location of this point
    PrecisionComplex location;
    
    // The computed orbit
    std::vector<ReferenceIteration> xn;
    
    // The first iteration where series approximation fails
    isize skipped = 0;
        
    // Indicates if the reference point has escaped
    bool escaped = false;
    
    // Norm at escape time
    double norm;

    
    //
    // Methods
    //
    
    ReferencePoint() { }
    ReferencePoint(const class Options &opt, Coord c);
        
    // Returns the relative location of a given pixel
    ExtendedComplex deltaLocation(const Options &opt, const Coord &coord) const;

    // Drills this reference point
    void drill(const Options &opt);
};

}
