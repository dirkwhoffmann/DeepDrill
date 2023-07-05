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

#include "DynamicFloat.h"
#include "Options.h"
#include "spline.h"

namespace dd {

void
DynamicFloat::init(std::vector<float> vxn, std::vector<float> vyn)
{
    xn.clear();
    for (const auto &it : vxn) xn.push_back(double(it));
    yn.clear();
    for (const auto &it : vyn) yn.push_back(double(it));

    if (xn.size() >= 3) {
        
        spline = tk::spline(xn, yn, tk::spline::cspline, true);
    }
}

std::ostream&
DynamicFloat::print(std::ostream& os) const
{
    if (yn.size() == 1) {

        os << yn[0];

    } else {

        for (usize i = 0; i < xn.size(); i++) {

            if (i) os << ", ";
            os << xn[i] << '/' << yn[i];
        }
    }

    return os;
}

float
DynamicFloat::operator() (double x) const
{
    assert(xn.size() != 0);
    assert(xn.size() != 2);
    assert(yn.size() != 0);
    assert(yn.size() != 2);
    return yn.size() == 1 ? float(yn[0]) : float(spline(x));
}

float
DynamicFloat::operator() (isize x) const
{
    return (*this)(double(x) / Options::video.frameRate);
}

}
