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

#include <SFML/Graphics.hpp>

namespace dd {

class Filter {

public: // REMOVE ASAP
    
    // Configuration options
    const struct Options &opt;

    // The compute kernel
    fs::path path;
    sf::Shader shader;

    // List of uniforms
    std::vector<string> uniforms;
    
    // Dimensions of the output texture
    isize width;
    isize height;

    // The output texture
    sf::RenderTexture out;
    sf::RectangleShape rect;


    //
    // Initializing
    //

public:

    Filter(const Options &opt) : opt(opt) { };
    void init(const string &shaderName, isize w, isize h);
    void init(const string &shaderName, sf::Vector2u resolution);

    const fs::path &getPath() const { return path; }
    const sf::Texture &getTexture() const { return out.getTexture(); }
    const sf::RectangleShape &getRect() const { return rect; }
    const sf::Vector2u getSize() const { return out.getSize(); }


    //
    // Managing uniforms
    //

public:

    bool hasUniform(const string &name);
    void setUniform(const string &key, float value);
    void setUniform(const string &key, sf::Vector2f value);
    void setUniform(const string &key, sf::Vector3f value);
    void setUniform(const string &key, const sf::Texture &value);


    //
    // Applying
    //

public:

    void apply();
};

}
