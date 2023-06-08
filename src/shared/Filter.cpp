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
Filter::init(const string &shaderName, isize w, isize h)
{
    init(shaderName, sf::Vector2u(unsigned(w), unsigned(h)));
}

void
Filter::init(const string &shaderName, sf::Vector2u resolution)
{
    width = resolution.x;
    height = resolution.y;

    // Create texture
    if (!out.create(resolution.x, resolution.y)) {
        throw Exception("Can't create render texture");
    }
    out.setSmooth(false);
    rect.setSize(sf::Vector2f(resolution));
    rect.setTexture(&out.getTexture());

    // Load shader
    path = opt.assets.findAsset(shaderName, Format::GLSL);
    
    if (path == "") {
        throw Exception("Can't load fragment shader '" + shaderName + "'");
    }
    if (!shader.loadFromFile(path, sf::Shader::Fragment)) {
        throw Exception("Can't load fragment shader '" + path.string() + "'");
    }

    // Extract all uniforms from the shader source
    std::ifstream infile(path);
    std::string line;
    while (std::getline(infile, line)) {
        if (line.rfind("uniform", 0) == 0) {
            line.pop_back();
            uniforms.push_back(line.substr(line.find_last_of(' ') + 1));
        }
    }
}

bool
Filter::hasUniform(const string &key)
{
    if constexpr (GPU_DEBUG) {
        return true;
    } else {
        return std::find(uniforms.begin(), uniforms.end(), key) != uniforms.end();
    }
}

void
Filter::setUniform(const string &key, float value)
{
    if (hasUniform(key)) shader.setUniform(key, value);
}

void
Filter::setUniform(const string &key, sf::Vector2f value)
{
    if (hasUniform(key)) shader.setUniform(key, value);
}

void
Filter::setUniform(const string &key, sf::Vector3f value)
{
    if (hasUniform(key)) shader.setUniform(key, value);
}

void
Filter::setUniform(const string &key, const sf::Texture &value)
{
    if (hasUniform(key)) shader.setUniform(key, value);
}

void
Filter::apply()
{
    out.draw(rect, &shader);
    out.display();
}

}
