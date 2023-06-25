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

std::ostream& operator<<(std::ostream& os, const Coord& value)
{
    os << "(" << value.x << "," << value.y << ")";
    return os;
}

}
