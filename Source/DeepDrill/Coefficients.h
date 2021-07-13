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

#include "MathTypes.h"
#include "ReferencePoint.h"

namespace dd {

class Coefficients {

    // Number of stored iterations
    isize iterations = 0;
    
    // Number of stored coefficients
    isize num = 0;
    
    // Computed coefficients
    ExtendedComplex *coeff = nullptr;
    
public:
    
    // Returns a pointer to the coefficients for a certain iteration
    ExtendedComplex *operator [] (const isize &) const;

    // Computes all coefficients
    void compute(ReferencePoint &ref, isize num, isize depth);
        
    // Computes an approximation
    ExtendedComplex evaluate(const Coord &coord, const ExtendedComplex &delta, isize iteration) const;

};

}
