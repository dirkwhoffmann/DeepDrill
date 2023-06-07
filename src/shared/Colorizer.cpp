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

namespace dd {

void
Colorizer::init()
{
    // Only initialize once
    if (image.getSize().x != 0) {
        printf("ALREADY INITIALIZED\n");
        return;
    } else {
        printf("NOT YET INITIALIZED\n");
    }

    // Get resolutions
    auto mapDim = sf::Vector2u(unsigned(opt.drillmap.width), unsigned(opt.drillmap.height));
    auto imageDim = sf::Vector2u(unsigned(opt.image.width), unsigned(opt.image.height));

    // Init GPU filters
    illuminator.init(opt.image.illuminator, mapDim);
    downscaler.init(opt.image.scaler, imageDim);
}

void
Colorizer::draw(const ColorMap &map)
{
    init();

    // 1. Apply lighting
    illuminator.apply([this, &map](sf::Shader &shader) {

        auto a = opt.colors.alpha * 3.14159 / 180.0;
        auto b = opt.colors.beta * 3.14159 / 180.0;

        auto z = std::sin(b);
        auto l = std::cos(b);
        auto y = std::sin(a) * l;
        auto x = std::cos(a) * l;

        shader.setUniform("lightDir", sf::Vector3f(x, y, z));
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
    image = illuminator.getTexture().copyToImage();
}

void
Colorizer::save(const string &path, Format format) const
{
    ProgressIndicator progress("Saving image data");

    image.saveToFile(path);
}

}
