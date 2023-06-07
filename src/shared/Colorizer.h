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
#include "ColorMap.h"
#include "Filter.h"
#include <functional>
#include <SFML/Graphics.hpp>

namespace dd {

class Colorizer {

    // Configuration options
    const struct Options &opt;

    // GPU filters
    Filter illuminator = Filter(opt);
    Filter illuminator2 = Filter(opt);

public:
    
    Filter downscaler = Filter(opt);
    Filter videoScaler = Filter(opt);

public:

    // Final image
    sf::Image image;


    //
    // Initializing
    //

public:

    Colorizer(const Options &opt) : opt(opt) { };
    ~Colorizer() { };

private:

    void init();


    //
    // Colorizing
    //

public:

    // Colorizes a still image
    void draw(DrillMap &map);
    void draw(const ColorMap &map);

    // Colorizes a video frame
    void draw(const ColorMap &map1, ColorMap &map2, float frame, float zoom);

private:

    sf::Vector3f lightVector();


    //
    // Exporting
    //

public:

    void save(const string &path, Format format) const;
};

}
