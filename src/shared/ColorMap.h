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

    // The normal map
    Buffer <u32> normalMap;

    // The color palette
    Palette palette = Palette(opt);

    // Textures
    sf::Texture colorMapTex;
    sf::Texture normalMapTex;

    // GPU filters
    Filter illuminator = Filter(opt);
    Filter downscaler = Filter(opt);


    sf::RenderTexture finalTex;
    sf::RectangleShape sourceRect;
    sf::RectangleShape targetRect;

    // Compute kernels
    sf::Shader scaler;

    // Final image
    sf::Image final;


    //
    // Initialization
    //

public:

    ColorMap(const Options &opt) : opt(opt) { };
    ~ColorMap();

    void resize(isize w, isize h);

    void update(const class DrillMap &map);

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

    const sf::Texture &getTexture() { return illuminator.getTexture(); }

    // Composes the final image
    sf::Image &computeImage();



    //
    // Exporting
    //

public:

    void save(const string &path, Format format);
};

}
