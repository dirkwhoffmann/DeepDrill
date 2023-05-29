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

#include "Coefficients.h"
#include "ProgressIndicator.h"
#include "ExtendedComplex.h"

namespace dd {

void
CoeffArray::resize(isize newRows, isize newCols)
{
    if (coeff) delete[] coeff;

    rows = newRows;
    cols = newCols;

    coeff = new ExtendedComplex[rows * cols] {};
}

ExtendedComplex *
CoeffArray::operator [] (const isize &index) const
{
    assert(index < rows);
    return coeff + (index * cols);
}

ExtendedComplex
CoeffArray::evaluate(const Coord &coord, const ExtendedComplex &delta, isize iteration) const
{
    ExtendedComplex *c = (*this)[iteration];
    ExtendedComplex approx = c[cols - 1];

    assert(delta.isReduced());

    // Apply Horner's method
    for (isize i = cols - 2; i >= 0; i--) {

        approx *= delta;
        approx += c[i];
        approx.reduce();
    }

    approx *= delta;
    approx.reduce();

    return approx;
}

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
    a.resize(depth, numCoeff);
    b.resize(depth, numCoeff);
    a[0][0] = ExtendedComplex(1, 0);

    // Based on the formulas from:
    // https://fractalwiki.org/wiki/Series_approximation
        
    for (isize i = 1; i < limit; i++) {
        
        assert(i < (isize)ref.xn.size());
        (*this)[i][0] = (*this)[i-1][0] * ref.xn[i-1].extended * (double)2;
        (*this)[i][0] += ExtendedComplex(1.0, 0.0);
        (*this)[i][0].reduce();
        a[i][0] = a[i-1][0] * ref.xn[i-1].extended * (double)2;
        a[i][0] += ExtendedComplex(1.0, 0.0);
        a[i][0].reduce();

        for (isize j = 1; j < numCoeff; j++) {
            
            (*this)[i][j] = (*this)[i-1][j] * ref.xn[i-1].extended * (double)2;
            (*this)[i][j].reduce();

            for (isize l = 0; l < j; l++) {
                (*this)[i][j] += (*this)[i-1][l] * (*this)[i-1][j-1-l];
                (*this)[i][j].reduce();
            }
            a[i][j] = a[i-1][j] * ref.xn[i-1].extended * (double)2;
            a[i][j].reduce();

            for (isize l = 0; l < j; l++) {
                a[i][j] += a[i-1][l] * a[i-1][j-1-l];
                a[i][j].reduce();
            }
        }
        
        // Update the progress counter
        if (i % 1024 == 0) progress.step(1024);
    }

    // Coefficients for the derivate
    for (isize i = 1; i < limit; i++) {

        assert(i < (isize)ref.xn.size());

        for (isize j = 1; j < numCoeff; j++) {

            b[i][j] = b[i-1][j] * ref.xn[i-1].extended;
            b[i][j].reduce();
            b[i][j] += a[i-1][j] * ref.xn[i-1].derivation;
            b[i][j].reduce();

            for (isize l = 0; l < j; l++) {
                b[i][j] += a[i-1][l] * b[i-1][j-1-l];
                b[i][j].reduce();
            }

            b[i][j] *= 2.0;
        }

        // Update the progress counter
        if (i % 1024 == 0) progress.step(1024);
    }

    // REMOVE ASAP
    for (isize i = 0; i < depth; i++) {
        for (isize j = 0; j < numCoeff; j++) {
            assert((*this)[i][j] == a[i][j]);
        }
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
