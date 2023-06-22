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

Coord
Coord::ul(const struct Options &opt)
{
    return Coord(0L, 0L);
}

Coord
Coord::ur(const struct Options &opt)
{
    return Coord(opt.drillmap.width - 1, 0);
}

Coord
Coord::ll(const struct Options &opt)
{
    return Coord(0, opt.drillmap.height - 1);
}

Coord
Coord::lr(const struct Options &opt)
{
    return Coord(opt.drillmap.width - 1, opt.drillmap.height - 1);
}

Coord
Coord::center(const Options &opt)
{
    return Coord(opt.drillmap.width / 2, opt.drillmap.height / 2);
}

/*
PrecisionComplex
Coord::translate(const Options &opt) const
{
    auto c = center(opt);
    
    // Compute the pixel distance to the center
    auto dx = opt.mpfPixelDeltaX * (x - c.x);
    auto dy = opt.mpfPixelDeltaY * (y - c.y);

    return opt.center + PrecisionComplex(dx, dy);
}
*/

std::ostream& operator<<(std::ostream& os, const Coord& value)
{
    os << "(" << value.x << "," << value.y << ")";
    return os;
}

}
