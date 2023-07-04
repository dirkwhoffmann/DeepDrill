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

class ImageMaker {

    // Configuration options
    const struct Options &opt;

    // GPU filters
    Filter colorizer = Filter(opt);
    Filter colorizer2 = Filter(opt);
    Filter illuminator = Filter(opt);
    Filter illuminator2 = Filter(opt);
    Filter downscaler = Filter(opt);

    // Final image
    sf::Image image;


    //
    // Initializing
    //

public:

    ImageMaker(const Options &opt) : opt(opt) { };
    ~ImageMaker() { };

    void init(const string &colorizationFilter, const string &illuminationFilter, const string &scalingFilter);

    //
    // Colorizing
    //

public:

    // Colorizes a still image
    void draw(DrillMap &map);
    void draw(DrillMap &dmap, const ColorMap &map);

    // Colorizes a video frame
    void draw(DrillMap &dmap1, DrillMap &dmap2, isize frame, float zoom);
    void draw(DrillMap &dmap1, DrillMap &dmap2, const ColorMap &map1, const ColorMap &map2, isize frame, float zoom);

private:

    sf::Vector3f lightVector(isize frame);


    //
    // Exporting
    //

public:

    // Returns the computed image or it's drawing shape
    const sf::RectangleShape &getRect() { return downscaler.getRect(); }
    const sf::Image &getImage() { return image; }

    // Exports the computed image to a file
    void save(const string &path, Format format) const;
};

}
