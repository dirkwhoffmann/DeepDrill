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
#include "PrecisionComplex.h"
#include "StandardComplex.h"
#include <iostream>

namespace dd {

PrecisionComplex &
PrecisionComplex::operator=(const StandardComplex &other)
{
    re = other.re;
    im = other.im;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const PrecisionComplex& value)
{
    os << "(" << value.re.get_d();
    os << "," << value.im.get_d() << "i";
    os << ")";
 
    return os;
}

}
