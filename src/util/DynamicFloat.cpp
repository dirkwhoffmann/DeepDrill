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

DynamicFloat::~DynamicFloat()
{
    delete (tk::spline *)pSpline;
}

void
DynamicFloat::init(std::vector<float> vxn, std::vector<float> vyn)
{
    assert(vxn.size() == vyn.size());

    xn.clear();
    for (const auto &it : vxn) xn.push_back(double(it));
    yn.clear();
    for (const auto &it : vyn) yn.push_back(double(it));

    // A spline needs at least three data points
    if (xn.size() == 2) {
        throw Exception("A spline description requires at least three data points");
    }

    // Abscissae must be in ascending order
    for (isize i = 0; i < (isize)xn.size() - 1; i++) {
        if (xn[i] >= xn[i + 1]) {
            throw Exception("Data points must be arranged in ascending order");
        }
    }

    // Create a spline if at least three data points are given
    if (xn.size() >= 3) {

        pSpline = (void *)new tk::spline(xn, yn, tk::spline::cspline, true);
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

    if (yn.size() == 1) {
        return float(yn[0]);
    } else {
        tk::spline *s = (tk::spline *)pSpline;
        return float((*s)(x));
    }
}

float
DynamicFloat::operator() (isize x) const
{
    return (*this)(double(x) / Options::video.frameRate);
}

}
