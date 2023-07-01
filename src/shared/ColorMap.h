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
        
    // The colorized drill map (DEPRECATED)
    std::vector<u32> colorMap;
    sf::Texture colorMapTex;

    // The color index map (EXPERIMENTAL)
    std::vector<u32> indexMap;
    sf::Texture indexMapTex;

    // The normal map
    std::vector<u32> normalMap;
    sf::Texture normalMapTex;

    // The color palette
    Palette palette = Palette(opt);
    sf::Texture paletteTex;


    //
    // Initialization
    //

public:

    ColorMap(const Options &opt) : opt(opt) { };

private:

    void resize(isize w, isize h);


    //
    // Colorizing
    //

public:

    void compute(const class DrillMap &map);
    template <ColoringMode> void compute(const class DrillMap &map);
};

}
