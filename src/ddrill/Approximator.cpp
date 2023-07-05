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

#include "Approximator.h"
#include "ProgressIndicator.h"
#include "ExtendedComplex.h"
#include "Options.h"

namespace dd {

void
Coefficients::resize(isize newRows, isize newCols)
{
    if (coeff) delete[] coeff;

    rows = newRows;
    cols = newCols;

    coeff = new ExtendedComplex[rows * cols] {};
}

ExtendedComplex *
Coefficients::operator [] (const isize &index) const
{
    assert(index < rows);
    return coeff + (index * cols);
}

ExtendedComplex
Coefficients::evaluate(const Coord &coord, const ExtendedComplex &delta, isize iteration) const
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

ExtendedComplex
Coefficients::evaluateDerivate(const Coord &coord, const ExtendedComplex &delta, isize iteration) const
{
    ExtendedComplex *c = (*this)[iteration];
    ExtendedComplex approx = c[cols - 1];
    approx *= double(cols);

    assert(delta.isReduced());

    // Apply Horner's method
    for (isize i = cols - 2; i >= 0; i--) {

        approx *= delta;
        approx += c[i] * double(i + 1);
        approx.reduce();
    }

    return approx;
}

void
Approximator::compute(ReferencePoint &ref, isize numCoeff, isize depth)
{
    assert(numCoeff >= 2 && numCoeff <= 64);

    auto limit = std::min(depth, (isize)ref.xn.size());

    ProgressIndicator progress("Computing coefficients", limit);

    a.resize(depth, numCoeff);

    // Based on the formulas from:
    // https://fractalwiki.org/wiki/Series_approximation

    a[0][0] = ExtendedComplex(1, 0);
    for (isize i = 1; i < limit; i++) {

        assert(i < (isize)ref.xn.size());
        a[i][0] = a[i-1][0] * ref.xn[i-1].extended * (double)2;
        a[i][0] += ExtendedComplex(1.0, 0.0);
        a[i][0].reduce();

        for (isize j = 1; j < numCoeff; j++) {

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

    progress.done();

    if (Options::flags.verbose) {

        log::cout << log::vspace;
        log::cout << log::ralign("Coefficients: ");
        log::cout << Options::approximation.coefficients << log::endl;
        log::cout << log::ralign("Approximation tolerance: ");
        log::cout << Options::approximation.tolerance << log::endl;
        log::cout << log::vspace;
    }
}

ExtendedComplex
Approximator::evaluate(const Coord &coord, const ExtendedComplex &delta, isize iteration) const
{
    return a.evaluate(coord, delta, iteration);
}

ExtendedComplex
Approximator::evaluateDerivate(const Coord &coord, const ExtendedComplex &delta, isize iteration) const
{
    return a.evaluateDerivate(coord, delta, iteration);
}

}
