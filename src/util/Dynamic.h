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

namespace dd {

struct Dynamic {

    std::vector<double> xn;
    std::vector<double> yn;

    class tk::spline spline;


    //
    // Initializing
    //

    Dynamic() { };
    void init(std::vector <double> xn, std::vector <double> yn);


    //
    // Operators
    //

    friend std::ostream& operator<<(std::ostream& os, const Dynamic& d);
    double operator() (double x) const;
};

}
