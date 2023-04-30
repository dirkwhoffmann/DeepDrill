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
#include "Buffer.h"

namespace dd {

class Palette {

    // Configuration options
    const struct Options &opt;
    
    Buffer <double> r;
    Buffer <double> g;
    Buffer <double> b;

public:

    Palette(const Options &options);

    void load(const string &path);
    u32 interpolateABGR(double value);
};

}
