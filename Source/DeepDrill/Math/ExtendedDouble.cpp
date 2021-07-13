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
#include "Types.h"
#include "ExtendedDouble.h"
#include <iostream>

namespace dd {

ExtendedDouble::ExtendedDouble(double m)
{
    mantissa = m;
    exponent = 0;
    reduce();
}

ExtendedDouble::ExtendedDouble(const mpf_class &value)
{
    mantissa = mpf_get_d_2exp(&exponent, value.get_mpf_t());
}

ExtendedDouble &
ExtendedDouble::operator=(const mpf_class &other)
{
    mantissa = mpf_get_d_2exp(&exponent, other.get_mpf_t());
    return *this;
}

bool
ExtendedDouble::operator==(const ExtendedDouble &other)
{
    assert(isReduced());
    assert(other.isReduced());
    
    return mantissa == other.mantissa && exponent == other.exponent;
}

std::ostream& operator<<(std::ostream& os, const ExtendedDouble& value)
{
    return os << value.mantissa << "*2^" << value.exponent;
}

}
