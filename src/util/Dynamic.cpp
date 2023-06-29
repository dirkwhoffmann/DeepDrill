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

#include "Dynamic.h"
#include "spline.h"

namespace dd {

void
Dynamic::init(std::vector <double> xn, std::vector <double> yn)
{
    spline = tk::spline(xn, yn);
}

double
Dynamic::get(double x)
{
    return spline(x);
}

}
