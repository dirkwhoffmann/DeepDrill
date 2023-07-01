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
}

void
Colorizer::draw(DrillMap &map)
{
    auto &colorMap = map.colorize();
    draw(colorMap);
}

void
Colorizer::draw(const ColorMap &map)
{
    {
        ProgressIndicator progress("Running GPU shaders");

        // 1. Apply lighting
        illuminator.setUniform("lightDir", lightVector());
        illuminator.setUniform("image", map.colorMapTex);
        illuminator.setUniform("normal", map.normalMapTex);
        illuminator.apply();

        // 2. Scale down
        downscaler.setUniform("curr", illuminator.getTexture());
        downscaler.setUniform("size", sf::Vector2f(illuminator.getSize()));
        downscaler.setUniform("zoom", 1.0f);
        downscaler.apply();

        // 3. Read back image data
        image = downscaler.getTexture().copyToImage();
    }

    if (opt.flags.verbose) {

        log::cout << log::vspace;
        log::cout << log::ralign("Illumination filter: ");
        log::cout << illuminator.getPath() << log::endl;
        log::cout << log::ralign("Downscaling filter: ");
        log::cout << downscaler.getPath() << log::endl;
        log::cout << log::vspace;
    }
}

void
Colorizer::draw(DrillMap &map1, DrillMap &map2, float zoom)
{
    auto &colorMap1 = map1.colorize();
    auto &colorMap2 = map2.colorize();
    draw(colorMap1, colorMap2, zoom);
}

void
Colorizer::draw(const ColorMap &map1, const ColorMap &map2, float zoom)
{
    // 1. Colorize
    illuminator.setUniform("lightDir", lightVector());
    illuminator.setUniform("image", map1.colorMapTex);
    illuminator.setUniform("index", map1.indexMapTex);
    illuminator.setUniform("palette", map1.paletteTex);
    illuminator.setUniform("normal", map1.normalMapTex);
    illuminator.apply();

    illuminator2.setUniform("lightDir", lightVector());
    illuminator2.setUniform("image", map2.colorMapTex);
    illuminator2.setUniform("index", map2.indexMapTex);
    illuminator2.setUniform("palette", map2.paletteTex);
    illuminator2.setUniform("normal", map2.normalMapTex);
    illuminator2.apply();

    // 2. Scale down
    downscaler.setUniform("curr", illuminator.getTexture());
    downscaler.setUniform("next", illuminator2.getTexture());
    downscaler.setUniform("size", sf::Vector2f(illuminator.getSize()));
    downscaler.setUniform("zoom", zoom);
    downscaler.apply();

    // 3. Read back image data
    image = downscaler.getTexture().copyToImage();
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
    {
        ProgressIndicator progress("Saving image");

        image.saveToFile(path);
    }
    if (opt.flags.verbose) {

        auto size = image.getSize();

        log::cout << log::vspace;
        log::cout << log::ralign("File name: ");
        log::cout << path << log::endl;
        log::cout << log::ralign("Image size: ");
        log::cout << isize(size.x) << " x " << isize(size.y) << log::endl;
        log::cout << log::vspace;
    }
}

}
