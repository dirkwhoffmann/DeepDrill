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

    // GPU filters
    Filter illuminator = Filter(opt); // DEPRECATED
    Filter downscaler = Filter(opt); // DEPRECATED


    sf::RenderTexture finalTex; // DEPRECATED
    sf::RectangleShape sourceRect; // DEPRECATED
    sf::RectangleShape targetRect; // DEPRECATED

    // Compute kernels
    sf::Shader scaler; // DEPRECATED

    // Final image
    sf::Image final; // DEPRECATED


    //
    // Initialization
    //

public:

    ColorMap(const Options &opt) : opt(opt) { };
    ~ColorMap();

    void resize(isize w, isize h);

private:

    // void init();
    void initTexture(sf::Texture &tex, sf::Vector2u size);
    void initTexture(sf::Texture &tex, sf::RectangleShape &rect, sf::Vector2u size);
    void initRenderTexture(sf::RenderTexture &tex, sf::Vector2u size);
    void initRenderTexture(sf::RenderTexture &tex, sf::RectangleShape &rect, sf::Vector2u size);
    void initShader(sf::Shader &shader, const string &name);

    
    //
    // Colorizing the Mandelbrot set
    //

public:

    void compute(const class DrillMap &map);

    const sf::Texture &getTexture() { return illuminator.getTexture(); }

    // Composes the final image (DEPRECATED)
    sf::Image &computeImage();



    //
    // Exporting
    //

public:

    void save(const string &path, Format format) const;
};

}
