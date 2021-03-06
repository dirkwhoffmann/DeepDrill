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

#include "config.h"
#include "Coefficients.h"
#include "ProgressIndicator.h"

namespace dd {

ExtendedComplex *
Coefficients::operator [] (const isize &index) const
{
    assert(index < iterations);
    return coeff + (index * num);
}

void
Coefficients::compute(ReferencePoint &ref, isize numCoeff, isize depth)
{
    assert(numCoeff >= 2 && numCoeff <= 64);

    num = numCoeff;
    iterations = depth;
    auto limit = std::min(depth, (isize)ref.xn.size());

    ProgressIndicator progress("Computing coefficients", limit);
    
    if (coeff) delete [] coeff;
    coeff = new ExtendedComplex[numCoeff * depth] {};
    (*this)[0][0] = ExtendedComplex(1, 0);

    // Based on the formulas from:
    // https://fractalwiki.org/wiki/Series_approximation
        
    for (isize i = 1; i < limit; i++) {
        
        assert(i < (isize)ref.xn.size());
        (*this)[i][0] = (*this)[i-1][0] * ref.xn[i-1].extended * (double)2;
        (*this)[i][0] += ExtendedComplex(1.0, 0.0);
        (*this)[i][0].reduce();
        
        for (isize j = 1; j < numCoeff; j++) {
            
            (*this)[i][j] = (*this)[i-1][j] * ref.xn[i-1].extended * (double)2;
            (*this)[i][j].reduce();
            for (isize l = 0; l < j; l++) {
                (*this)[i][j] += (*this)[i-1][l] * (*this)[i-1][j-1-l];
                (*this)[i][j].reduce();
            }
        }
        
        // Update the progress counter
        if (i % 1024 == 0) progress.step(1024);
    }
}

ExtendedComplex
Coefficients::evaluate(const Coord &coord, const ExtendedComplex &delta, isize iteration) const {
    
    ExtendedComplex *c = (*this)[iteration];
    ExtendedComplex approx = c[num - 1];

    assert(delta.isReduced());
    
    // Apply Horner's method
    for (isize i = num - 2; i >= 0; i--) {
        
        approx *= delta;
        approx += c[i];
        approx.reduce();
    }
    
    approx *= delta;
    approx.reduce();
    
    return approx;
}

}
