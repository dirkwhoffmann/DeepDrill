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

    // Path to the palette image
    fs::path path;

    // Color values
    Buffer <double> r;
    Buffer <double> g;
    Buffer <double> b;

public:

    Palette(const Options &options);

    const fs::path &getPath() { return path; }
    void load(const string &filename);
    u32 interpolateABGR(double value);
};

}
