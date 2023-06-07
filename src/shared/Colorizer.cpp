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

#include "Colorizer.h"
#include "Exception.h"
#include "ProgressIndicator.h"
#include "DrillMap.h"

namespace dd {

void
Colorizer::init(const string &illuminationFilter, const string &scalingFilter)
{
    // Only initialize once
    assert(illuminator.getSize().x == 0);

    // Get resolutions
    auto mapDim = sf::Vector2u(unsigned(opt.drillmap.width), unsigned(opt.drillmap.height));
    auto imageDim = sf::Vector2u(unsigned(opt.image.width), unsigned(opt.image.height));

    // Init GPU filters
    illuminator.init(illuminationFilter, mapDim);
    illuminator2.init(illuminationFilter, mapDim);
    downscaler.init(scalingFilter, imageDim);
    videoScaler.init(scalingFilter, imageDim);
}

/*
void
Colorizer::init()
{
    // Only initialize once
    if (illuminator.getSize().x != 0) return;

    // Get resolutions
    auto mapDim = sf::Vector2u(unsigned(opt.drillmap.width), unsigned(opt.drillmap.height));
    auto imageDim = sf::Vector2u(unsigned(opt.image.width), unsigned(opt.image.height));

    // Init GPU filters
    illuminator.init(opt.image.illuminator, mapDim);
    illuminator2.init(opt.image.illuminator, mapDim);
    downscaler.init(opt.image.scaler, imageDim);
    videoScaler.init(opt.video.scaler, imageDim);
}
*/

void
Colorizer::draw(DrillMap &map)
{
    auto &colorMap = map.colorize();
    draw(colorMap);
}

void
Colorizer::draw(const ColorMap &map)
{
    // 1. Apply lighting
    illuminator.apply([this, &map](sf::Shader &shader) {

        shader.setUniform("lightDir", lightVector());
        shader.setUniform("image", map.colorMapTex);
        shader.setUniform("normal", map.normalMapTex);
    });

    // 2. Scale down
    downscaler.apply([this](sf::Shader &shader) {

        shader.setUniform("curr", illuminator.getTexture());
        shader.setUniform("size", sf::Vector2f(illuminator.getSize()));
        shader.setUniform("zoom", 1.0f);
        shader.setUniform("frame", 0.0f);
    });

    // 3. Read back image data
    image = downscaler.getTexture().copyToImage();
}

void
Colorizer::draw(DrillMap &map1, DrillMap &map2, float frame, float zoom)
{
    auto &colorMap1 = map1.colorize();
    auto &colorMap2 = map2.colorize();
    draw(colorMap1, colorMap2, frame, zoom);
}

void
Colorizer::draw(const ColorMap &map1, const ColorMap &map2, float frame, float zoom)
{
    // 1. Colorize
    illuminator.apply([this, &map1](sf::Shader &shader) {

        shader.setUniform("lightDir", lightVector());
        shader.setUniform("image", map1.colorMapTex);
        shader.setUniform("normal", map1.normalMapTex);
    });
    illuminator2.apply([this, &map2](sf::Shader &shader) {

        shader.setUniform("lightDir", lightVector());
        shader.setUniform("image", map2.colorMapTex);
        shader.setUniform("normal", map2.normalMapTex);
    });

    // 2. Scale down
    videoScaler.apply([this, &zoom, &frame](sf::Shader &shader) {

        shader.setUniform("curr", illuminator.getTexture());
        shader.setUniform("next", illuminator2.getTexture());
        shader.setUniform("size", sf::Vector2f(illuminator.getSize()));
        shader.setUniform("zoom", zoom);
        shader.setUniform("frame", frame);
    });

    // 3. Read back image data
    image = videoScaler.getTexture().copyToImage();
}

sf::Vector3f
Colorizer::lightVector()
{
    auto a = opt.colors.alpha * 3.14159 / 180.0;
    auto b = opt.colors.beta * 3.14159 / 180.0;
    auto z = std::sin(b);
    auto l = std::cos(b);
    auto y = std::sin(a) * l;
    auto x = std::cos(a) * l;

    return sf::Vector3f(x, y, z);
}

void
Colorizer::save(const string &path, Format format) const
{
    ProgressIndicator progress("Saving image data");

    image.saveToFile(path);
}

}
