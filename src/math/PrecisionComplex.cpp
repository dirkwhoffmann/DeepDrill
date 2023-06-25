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

#include "PrecisionComplex.h"
#include "StandardComplex.h"
#include <iostream>

namespace dd {

std::ostream& operator<<(std::ostream& os, const PrecisionComplex& c)
{
    os << StandardComplex(c);
    return os;
}

bool
PrecisionComplex::inCardioid()
{
    mpf_class r1 = re + 1.0;
    mpf_class ii = im * im;

    return r1 * r1 + ii < 0.0625;
};

bool
PrecisionComplex::inMainBulb()
{
    mpf_class ii = im * im;
    mpf_class p = re - 0.25;
    mpf_class q = p * p + ii;

    return q * (q + p) < ii * 0.25;
};

}
