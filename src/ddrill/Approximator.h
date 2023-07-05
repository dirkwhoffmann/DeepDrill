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
#include "ReferencePoint.h"

namespace dd {

class Coefficients {

    isize rows = 0;
    isize cols = 0;
    ExtendedComplex *coeff = nullptr;

public:

    void resize(isize newRows, isize newCols);
    ExtendedComplex *operator [] (const isize &) const;
    ExtendedComplex evaluate(const Coord &coord, const ExtendedComplex &delta, isize iteration) const;
    ExtendedComplex evaluateDerivate(const Coord &coord, const ExtendedComplex &delta, isize iteration) const;
};

class Approximator {

    // Coefficient array
    Coefficients a;


    //
    // Initializing
    //

public:

    Approximator() { };


    //
    // Computing
    //

public:

    void compute(ReferencePoint &ref, isize num, isize depth);


    //
    // Evaluating
    //

public:

    ExtendedComplex evaluate(const Coord &coord, const ExtendedComplex &delta, isize iteration) const;
    ExtendedComplex evaluateDerivate(const Coord &coord, const ExtendedComplex &delta, isize iteration) const;
};

}
