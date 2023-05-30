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

#include "StandardComplex.h"
#include "ExtendedComplex.h"
#include "PrecisionComplex.h"

namespace dd {

StandardComplex::StandardComplex(const PrecisionComplex &other) :
re(other.re.get_d()), im(other.im.get_d())
{

}

StandardComplex::StandardComplex(const ExtendedComplex &other)
{
    re = ldexp(other.mantissa.re, (int)other.exponent);
    im = ldexp(other.mantissa.im, (int)other.exponent);
}

StandardComplex &
StandardComplex::operator=(const PrecisionComplex &other)
{
    re = other.re.get_d();
    im = other.im.get_d();
    
    return *this;
}

}
