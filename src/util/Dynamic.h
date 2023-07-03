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

template <class T> struct Dynamic {

    static double fps;

    std::vector<double> xn;
    std::vector<double> yn;

    class tk::spline spline;


    //
    // Initializing
    //

    Dynamic() { };
    void init(std::vector <T> xn, std::vector <T> yn);


    //
    // Operators
    //

    friend std::ostream& operator<<(std::ostream& os, const Dynamic<T>& d) { return d.print(os); }
    T operator() (double x) const;
    T operator() (isize x) const;

private:

    std::ostream& print(std::ostream& os) const;
};

}
