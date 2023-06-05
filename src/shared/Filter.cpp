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

#include "Filter.h"
// #include "AssetManager.h"
#include "Exception.h"
#include "Options.h"

namespace dd {

void
Filter::init(const string &shaderName, int w, int h)
{
    width = w;
    height = h;

    // Create texture
    if (!out.create(w, h)) {
        throw Exception("Can't create render texture");
    }
    out.setSmooth(false);
    rect.setSize(sf::Vector2f(w, h));
    rect.setTexture(&out.getTexture());


    // Load shader
    auto path = opt.assets.findAsset(shaderName, Format::GLSL);

    if (path == "") {
        throw Exception("Can't load fragment shader '" + shaderName + "'");
    }
    if (!shader.loadFromFile(path, sf::Shader::Fragment)) {
        throw Exception("Can't load fragment shader '" + path.string() + "'");
    }
}

void
Filter::apply(std::function<void (sf::Shader&)> setup)
{
    // Setup uniforms
    setup(shader);

    // Apply kernel
    out.draw(rect, &shader);
    out.display();
}

}
