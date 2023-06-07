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
#include "Filter.h"

#include <SFML/Graphics.hpp>

namespace dd {

class ColorMap {

    // Configuration options
    const struct Options &opt;

    // Resolution
    isize width = 0;
    isize height = 0;

public:
        
    // The colorized drill map
    Buffer <u32> colorMap;
    sf::Texture colorMapTex;

    // The normal map
    Buffer <u32> normalMap;
    sf::Texture normalMapTex;

    // The color palette
    Palette palette = Palette(opt);


    //
    // Initialization
    //

public:

    ColorMap(const Options &opt) : opt(opt) { };
    ~ColorMap();

private:

    void resize(isize w, isize h);


    //
    // Colorizing
    //

public:

    void compute(const class DrillMap &map);
};

}
