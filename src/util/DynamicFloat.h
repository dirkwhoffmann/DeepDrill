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

#pragma once

#include "config.h"
#include "Types.h"
#include "spline.h"

#pragma GCC diagnostic ignored "-Wunused-result"

namespace dd {

struct DynamicFloat {

    std::vector<double> xn;
    std::vector<double> yn;

    class tk::spline spline;


    //
    // Initializing
    //

    DynamicFloat() { };
    void init(std::vector<float> xn, std::vector<float> yn);


    //
    // Operators
    //

    friend std::ostream& operator<<(std::ostream& os, const DynamicFloat& d) { return d.print(os); }
    float operator() (double x = 0.0) const;
    float operator() (isize x) const;

private:

    std::ostream& print(std::ostream& os) const;
};

}
