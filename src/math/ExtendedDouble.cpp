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
ExtendedDouble::operator==(const ExtendedDouble &other) const
{
    assert(isReduced());
    assert(other.isReduced());
    
    return mantissa == other.mantissa && exponent == other.exponent;
}

bool
ExtendedDouble::operator<(ExtendedDouble &other)
{
    assert(isReduced());
    assert(other.isReduced());

    if (mantissa == 0.0 && other.mantissa < 0.0) {
        return false;
     } if (mantissa == 0.0 && other.mantissa > 0.0) {
         return true;
     } else if (other.mantissa == 0.0 && mantissa < 0.0) {
         return true;
     } else if (other.mantissa == 0.0 && mantissa > 0.0) {
         return false;
     } else if (exponent < other.exponent) {
         return true;
     } else if (exponent > other.exponent) {
         return false;
     } else if (mantissa == other.mantissa) {
         return false;
     } else if (mantissa < other.mantissa) {
         return true;
     } else {
         return false;
     }
}

bool
ExtendedDouble::operator>(ExtendedDouble &other)
{
    return other < *this;
}

bool
ExtendedDouble::operator<(double other)
{
    assert(isReduced());

    ExtendedDouble tmp(other);
    tmp.reduce();
    return *this < tmp;
}

bool
ExtendedDouble::operator>(double other)
{
    assert(isReduced());

    ExtendedDouble tmp(other);
    tmp.reduce();
    return *this > tmp;
}

}
