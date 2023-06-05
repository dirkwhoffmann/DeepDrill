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
#include <functional>
#include <SFML/Graphics.hpp>

namespace dd {

class Filter {

    // Configuration options
    const struct Options &opt;

    // The compute kernel
    sf::Shader shader;

    // The first input texture (mandatory)
    sf::Texture in1;

    // The second input texture (optional)
    sf::Texture in2;

    // Dimensions of the output texture
    isize width;
    isize height;

    // The output texture
    sf::RenderTexture out;
    sf::RectangleShape rect;

public:

    //
    // Initializing
    //

    Filter(const Options &opt) : opt(opt) { };
    void init(const string &shaderName, int w, int h);

    const sf::Texture &getTexture() { return out.getTexture(); }
    sf::RectangleShape &getRect() { return rect; }
    void setTextureRect(const sf::IntRect r) { rect.setTextureRect(r); }


    //
    // Applying
    //

    void apply(std::function<void (sf::Shader&)> setup);

};

}
