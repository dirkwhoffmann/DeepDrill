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
#include "Options.h"

namespace dd {

/*
Coord::Coord(PrecisionComplex pos, const Options &opt)
{
    auto c = opt.center;

    // Compute the pixel distance to the center
    mpf_class dx = (pos.re - c.re) / opt.mpfPixelDelta;
    mpf_class dy = (pos.im - c.im) / opt.mpfPixelDelta;

    x = center(opt).x + i16(round(dx.get_d()));
    y = center(opt).y + i16(round(dy.get_d()));
}
*/

Coord
Coord::center(const Options &opt)
{
    return Coord(opt.image.width / 2, opt.image.height / 2);
}

PrecisionComplex
Coord::translate(const Options &opt) const
{
    auto c = center(opt);
    
    // Compute the pixel distance to the center
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
