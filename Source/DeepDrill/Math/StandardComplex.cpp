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
#include "StandardComplex.h"
#include "PrecisionComplex.h"

namespace dd {

StandardComplex::StandardComplex(const PrecisionComplex &other) :
re(other.re.get_d()), im(other.im.get_d())
{

}

StandardComplex &
StandardComplex::operator=(const PrecisionComplex &other)
{
    re = other.re.get_d();
    im = other.im.get_d();
    
    return *this;
}

std::ostream& operator<<(std::ostream& os, const StandardComplex& value)
{
    os << "(" << value.re;
    os << "," << value.im << "i";
    os << ")";

    return os;
}

}
