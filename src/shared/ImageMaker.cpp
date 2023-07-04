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

#include "ImageMaker.h"
#include "Exception.h"
#include "ProgressIndicator.h"
#include "DrillMap.h"

namespace dd {

void
ImageMaker::init(const string &colorizationFilter, const string &illuminationFilter, const string &scalingFilter)
{
    // Only initialize once
    assert(illuminator.getSize().x == 0);

    // Get resolutions
    auto mapDim = sf::Vector2u(unsigned(opt.drillmap.width), unsigned(opt.drillmap.height));
    auto imageDim = sf::Vector2u(unsigned(opt.image.width), unsigned(opt.image.height));

    // Init GPU filters
    colorizer.init(colorizationFilter, mapDim);
    colorizer2.init(colorizationFilter, mapDim);
    illuminator.init(illuminationFilter, mapDim);
    illuminator2.init(illuminationFilter, mapDim);
    downscaler.init(scalingFilter, imageDim);
}

void
ImageMaker::draw(DrillMap &map)
{
    auto &colorMap = map.colorize();
    draw(map, colorMap);
}

void
ImageMaker::draw(DrillMap &dmap, const ColorMap &map)
{
    {
        ProgressIndicator progress("Running GPU shaders");

        // 1. Colorize
        colorizer.setUniform("iter", map.iterMapTex);
        colorizer.setUniform("overlay", map.overlayMapTex);
        colorizer.setUniform("texture", map.textureMapTex);
        colorizer.setUniform("lognorm", map.lognormMapTex);
        colorizer.setUniform("palette", map.paletteTex);
        colorizer.setUniform("normalRe", map.normalReMapTex);
        colorizer.setUniform("normalIm", map.normalImMapTex);
        colorizer.setUniform("paletteScale", opt.palette.scale());
        colorizer.setUniform("paletteOffset", opt.palette.offset());
        colorizer.setUniform("textureOpacity", opt.texture.image == "" ? 0.0 : opt.texture.opacity());
        colorizer.setUniform("textureScale", opt.texture.scale());
        colorizer.setUniform("textureOffset", opt.texture.offset());
        colorizer.apply();

        // 2. Illuminate
        illuminator.setUniform("image", colorizer.getTexture());
        illuminator.setUniform("lightDir", lightVector(0));
        illuminator.setUniform("iter", map.iterMapTex);
        illuminator.setUniform("overlay", map.overlayMapTex);
        illuminator.setUniform("texture", map.textureMapTex);
        illuminator.setUniform("lognorm", map.lognormMapTex);
        illuminator.setUniform("palette", map.paletteTex);
        illuminator.setUniform("normalRe", map.normalReMapTex);
        illuminator.setUniform("normalIm", map.normalImMapTex);
        illuminator.setUniform("paletteScale", opt.palette.scale());
        illuminator.setUniform("paletteOffset", opt.palette.offset());
        illuminator.setUniform("textureOpacity", opt.texture.image == "" ? 0.0 : opt.texture.opacity());
        illuminator.setUniform("textureScale", opt.texture.scale());
        illuminator.setUniform("textureOffset", opt.texture.offset());
        illuminator.apply();

        // 3. Scale down
        downscaler.setUniform("curr", illuminator.getTexture());
        downscaler.setUniform("size", sf::Vector2f(illuminator.getSize()));
        downscaler.setUniform("numTextures", 1);
        downscaler.setUniform("zoom", 1.0f);
        downscaler.apply();

        // 4. Read back image data
        image = downscaler.getTexture().copyToImage();
    }

    if (opt.flags.verbose) {

        log::cout << log::vspace;
        log::cout << log::ralign("Colorizer: ");
        log::cout << colorizer.getPath() << log::endl;
        log::cout << log::ralign("Illuminator: ");
        log::cout << illuminator.getPath() << log::endl;
        log::cout << log::ralign("Downscaler: ");
        log::cout << downscaler.getPath() << log::endl;
        log::cout << log::vspace;
    }
}

void
ImageMaker::draw(DrillMap &map1, DrillMap &map2, isize frame, float zoom)
{
    auto &colorMap1 = map1.colorize();
    auto &colorMap2 = map2.colorize();
    draw(map1, map2, colorMap1, colorMap2, frame, zoom);
}

void
ImageMaker::draw(DrillMap &dmap1, DrillMap &dmap2, const ColorMap &map1, const ColorMap &map2, isize frame, float zoom)
{
    // 1. Colorize
    colorizer.setUniform("iter", dmap1.getIterationMapTex());
    colorizer.setUniform("overlay", map1.overlayMapTex);
    colorizer.setUniform("texture", map1.textureMapTex);
    colorizer.setUniform("lognorm", dmap1.getLognormMapTex());
    colorizer.setUniform("palette", map1.paletteTex);
    colorizer.setUniform("normalRe", dmap1.getNormalReMapTex());
    colorizer.setUniform("normalIm", dmap1.getNormalImMapTex());
    colorizer.setUniform("paletteScale", opt.palette.scale(frame));
    colorizer.setUniform("paletteOffset", opt.palette.offset(frame));
    colorizer.setUniform("textureOpacity", opt.texture.image == "" ? 0.0 : opt.texture.opacity(frame));
    colorizer.setUniform("textureScale", opt.texture.scale(frame));
    colorizer.setUniform("textureOffset", opt.texture.offset(frame));
    colorizer.apply();

    colorizer2.setUniform("iter", dmap2.getIterationMapTex());
    colorizer2.setUniform("overlay", map2.overlayMapTex);
    colorizer2.setUniform("texture", map2.textureMapTex);
    colorizer2.setUniform("lognorm", dmap2.getLognormMapTex());
    colorizer2.setUniform("palette", map2.paletteTex);
    colorizer2.setUniform("normalRe", dmap2.getNormalReMapTex());
    colorizer2.setUniform("normalIm", dmap2.getNormalImMapTex());
    colorizer2.setUniform("paletteScale", opt.palette.scale(frame));
    colorizer2.setUniform("paletteOffset", opt.palette.offset(frame));
    colorizer2.setUniform("textureOpacity", opt.texture.image == "" ? 0.0 : opt.texture.opacity(frame));
    colorizer2.setUniform("textureScale", opt.texture.scale(frame));
    colorizer2.setUniform("textureOffset", opt.texture.offset(frame));
    colorizer2.apply();

    // 2. Illuminate
    illuminator.setUniform("image", colorizer.getTexture());
    illuminator.setUniform("lightDir", lightVector(frame));
    illuminator.setUniform("normalRe", map1.normalReMapTex);
    illuminator.setUniform("normalIm", map1.normalImMapTex);
    illuminator.apply();

    illuminator2.setUniform("image", colorizer2.getTexture());
    illuminator2.setUniform("lightDir", lightVector(frame));
    illuminator2.setUniform("normalRe", map2.normalReMapTex);
    illuminator2.setUniform("normalIm", map2.normalImMapTex);
    illuminator2.apply();

    // 3. Scale down
    downscaler.setUniform("curr", colorizer.getTexture());
    downscaler.setUniform("next", colorizer2.getTexture());
    downscaler.setUniform("size", sf::Vector2f(illuminator.getSize()));
    downscaler.setUniform("numTextures", 2);
    downscaler.setUniform("zoom", zoom);
    downscaler.apply();

    // 4. Read back image data
    image = downscaler.getTexture().copyToImage();
}

sf::Vector3f
ImageMaker::lightVector(isize frame)
{
    auto a = opt.lighting.alpha(frame) * 3.14159 / 180.0;
    auto b = opt.lighting.beta(frame) * 3.14159 / 180.0;
    auto z = std::sin(b);
    auto l = std::cos(b);
    auto y = std::sin(a) * l;
    auto x = std::cos(a) * l;

    return sf::Vector3f(x, y, z);
}

void
ImageMaker::save(const string &path, Format format) const
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
