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
#include "Palette.h"

namespace dd {

enum class ColorScheme { Default, Custom };

class Colorizer {

    // Configuration options
    const struct Options &opt;
    
    // The associated map file
    const class DrillMap &map;
    
    // The image data
    u32 *image = nullptr;

    // The color scheme
    ColorScheme scheme = ColorScheme::Default;

    // Custom color table
    vector <u32> colors;

    // The color palette
    Palette palette = Palette(opt);

    
    //
    // Initialization
    //

public:

    Colorizer(const Options &options, const DrillMap &map);
    ~Colorizer();
    
    
    //
    // Colorizing the Mandelbrot set
    //

    void colorize();

private:

    void colorize(struct Coord c);

    
    //
    // Exporting
    //

public:

    void save(const string &path);
};

}
