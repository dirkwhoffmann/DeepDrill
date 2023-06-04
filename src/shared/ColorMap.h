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
#include "Options.h"
#include "Buffer.h"
#include "Palette.h"

namespace dd {

class ColorMap {

    // Configuration options
    const struct Options &opt;

public:
        
    // The colorized drill map
    Buffer <u32> colorMap;

    // The normal map
    Buffer <u32> normalMap;

    // The color palette
    Palette palette = Palette(opt);


    //
    // Initialization
    //

public:

    ColorMap(const Options &opt) : opt(opt) { };
    ~ColorMap();

private:

    void resize(const class DrillMap &map);
    void resize(isize w, isize h);

    // [[deprecated]] void init();

    
    //
    // Colorizing the Mandelbrot set
    //

public:

    void compute(const class DrillMap &map);

private:

    void compute(const class DrillMap &map, struct Coord c);

    
    //
    // Exporting
    //

public:

    void save(const string &path, Format format);
};

}
