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

#include "Coord.h"

namespace dd {

Coord
Coord::center(const Options &opt)
{
    return Coord(opt.width / 2, opt.height / 2);
}

PrecisionComplex
Coord::translate(const Options &opt) const
{
    auto c = center(opt);
    
    // Compute the pixel delta to the center
    auto dx = opt.mpfPixelDelta * (x - c.x);
    auto dy = opt.mpfPixelDelta * (y - c.y);
        
    return opt.center + PrecisionComplex(dx, dy);
}

std::ostream& operator<<(std::ostream& os, const Coord& value)
{
    os << "(" << value.x << "," << value.y << ")";
    return os;
}

}
