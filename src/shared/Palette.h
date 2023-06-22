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

namespace dd {

class Palette {

    // Configuration options
    const struct Options &opt;

    // Path to the palette image
    fs::path imagePath;

    // Color values
    std::vector <double> r;
    std::vector <double> g;
    std::vector <double> b;

public:

    Palette(const Options &options);

    const fs::path &getPath() { return imagePath; }
    void load(const fs::path &path);
    u32 interpolateABGR(double value);
};

}
