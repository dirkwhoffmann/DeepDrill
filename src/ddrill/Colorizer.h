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

#include "commons.h"
#include "Palette.h"

namespace dd {

class Colorizer {

    // Configuration options
    const struct DrillOptions &opt;
    
    // The associated map file
    const class DrillMap &map;
    
    // The image data
    u32 *image = nullptr;

    // The color palette
    Palette palette = Palette(opt);

    // Path to the raw2tiff tool
    string raw2tiff;
    
    
    //
    // Initialization
    //

public:

    Colorizer(const DrillOptions &options, const DrillMap &map);
    ~Colorizer();
    
    
    //
    // Colorizing the Mandelbrot set
    //

    void colorize();
    void colorize(struct Coord c);

    
    //
    // Exporting
    //
    
    void save(const string &path);
};

}
