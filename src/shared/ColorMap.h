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
        
    // The iteration count map (EXPERIMENTAL)
    std::vector<u32> iterMap;
    sf::Texture iterMapTex;

    // The overlay map (EXPERIMENTAL)
    std::vector<u32> overlayMap;
    sf::Texture overlayMapTex;

    // The texture map (EXPERIMENTAL)
    // std::vector<u32> textureMap;
    sf::Texture textureMapTex;

    // The lognorm map (EXPERIMENTAL)
    std::vector<float> lognormMap;
    sf::Texture lognormMapTex;

    // The normal map
    std::vector<float> normalReMap;
    sf::Texture normalReMapTex;
    std::vector<float> normalImMap;
    sf::Texture normalImMapTex;

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
