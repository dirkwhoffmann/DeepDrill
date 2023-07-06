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
#include "AssetManager.h"
#include "DrillMap.h"
#include "Palette.h"
#include "Filter.h"
#include <functional>
#include <SFML/Graphics.hpp>

namespace dd {

class ImageMaker {

    // The color palette
    Palette palette = Palette();
    sf::Texture paletteTex;

    // An optional overlay texture
    sf::Texture textureMapTex;

    // GPU filters
    Filter colorizer;
    Filter colorizer2;
    Filter illuminator;
    Filter illuminator2;
    Filter downscaler;

    // Final image
    sf::Image image;


    //
    // Initializing
    //

private:

    void init();


    //
    // Colorizing
    //

public:

    // Colorizes a still image
    void draw(DrillMap &map);

    // Colorizes a video frame
    void draw(DrillMap &map1, DrillMap &map2, isize frame, float zoom);

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
