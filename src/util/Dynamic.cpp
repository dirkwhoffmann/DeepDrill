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
Dynamic::init(std::vector <double> vxn, std::vector <double> vyn)
{
    this->xn = vxn;
    this->yn = vyn;

    spline = tk::spline(xn, yn, tk::spline::cspline, true);
}

std::ostream &
operator<<(std::ostream& os, const Dynamic& d)
{
    if (d.yn.size() == 1) {

        os << d.yn[0];

    } else {

        for (usize i = 0; i < d.xn.size(); i++) {

            if (i) os << ", ";
            os << d.xn[i] << '/' << d.yn[i];
        }
    }

    return os;
}

double
Dynamic::operator() (double x) const
{
    return yn.size() == 1 ? yn[0] : spline(x);
}

}
