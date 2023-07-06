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
#include "Options.h"
#include "ProgressIndicator.h"
#include "DrillMap.h"

namespace dd {

void
ImageMaker::init(const string &colorizationFilter, const string &illuminationFilter, const string &scalingFilter)
{
    // Only initialize once
    assert(illuminator.getSize().x == 0);

    // Load the color palette
    if (!paletteTex.loadFromImage(palette.getImage())) {
        throw Exception("Failed to create palette texture");
    }

    // Load the texture overlay
    if (!textureMapTex.loadFromImage(palette.getTextureImage())) {
        throw Exception("Failed to create overlay texture");
    }

    // Setup GPU filters
    auto mapDim = sf::Vector2u(unsigned(Options::drillmap.width), unsigned(Options::drillmap.height));
    auto imageDim = sf::Vector2u(unsigned(Options::image.width), unsigned(Options::image.height));
    colorizer.init(colorizationFilter, mapDim);
    colorizer2.init(colorizationFilter, mapDim);
    illuminator.init(illuminationFilter, mapDim);
    illuminator2.init(illuminationFilter, mapDim);
    downscaler.init(scalingFilter, imageDim);
}

void
ImageMaker::draw(DrillMap &dmap)
{
    {
        ProgressIndicator progress("Running GPU shaders");

        // 1. Colorize
        colorizer.setUniform("iter", dmap.getIterationMapTex());
        colorizer.setUniform("lognorm", dmap.getLognormMapTex());
        colorizer.setUniform("palette", paletteTex);
        colorizer.setUniform("normalRe", dmap.getNormalReMapTex());
        colorizer.setUniform("normalIm", dmap.getNormalImMapTex());
        colorizer.setUniform("texture", textureMapTex);
        colorizer.setUniform("overlay", dmap.getOverlayMapTex());
        colorizer.setUniform("paletteScale", Options::palette.scale());
        colorizer.setUniform("paletteOffset", Options::palette.offset());
        colorizer.setUniform("textureOpacity", Options::texture.opacity());
        colorizer.setUniform("textureScale", Options::texture.scale());
        colorizer.setUniform("textureOffset", Options::texture.offset());
        colorizer.apply();

        // 2. Illuminate
        if (Options::lighting.enable) {

            illuminator.setUniform("image", colorizer.getTexture());
            illuminator.setUniform("lightDir", lightVector(0));
            illuminator.setUniform("normalRe", dmap.getNormalReMapTex());
            illuminator.setUniform("normalIm", dmap.getNormalImMapTex());
            illuminator.apply();

            downscaler.setUniform("curr", illuminator.getTexture());

        } else {

            downscaler.setUniform("curr", colorizer.getTexture());
        }

        // 3. Scale down
        downscaler.setUniform("size", sf::Vector2f(illuminator.getSize()));
        downscaler.setUniform("numTextures", 1);
        downscaler.setUniform("zoom", 1.0f);
        downscaler.apply();

        // 4. Read back image data
        image = downscaler.getTexture().copyToImage();
    }

    if (Options::flags.verbose) {

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
ImageMaker::draw(DrillMap &dmap1, DrillMap &dmap2, isize frame, float zoom)
{
    // 1. Colorize
    colorizer.setUniform("iter", dmap1.getIterationMapTex());
    colorizer.setUniform("lognorm", dmap1.getLognormMapTex());
    colorizer.setUniform("palette", paletteTex);
    colorizer.setUniform("normalRe", dmap1.getNormalReMapTex());
    colorizer.setUniform("normalIm", dmap1.getNormalImMapTex());
    colorizer.setUniform("texture", textureMapTex);
    colorizer.setUniform("overlay", dmap1.getOverlayMapTex());
    colorizer.setUniform("paletteScale", Options::palette.scale(frame));
    colorizer.setUniform("paletteOffset", Options::palette.offset(frame));
    colorizer.setUniform("textureOpacity", Options::texture.image == "" ? 0.0 : Options::texture.opacity(frame));
    colorizer.setUniform("textureScale", Options::texture.scale(frame));
    colorizer.setUniform("textureOffset", Options::texture.offset(frame));
    colorizer.apply();

    colorizer2.setUniform("iter", dmap2.getIterationMapTex());
    colorizer2.setUniform("lognorm", dmap2.getLognormMapTex());
    colorizer2.setUniform("palette", paletteTex);
    colorizer2.setUniform("normalRe", dmap2.getNormalReMapTex());
    colorizer2.setUniform("normalIm", dmap2.getNormalImMapTex());
    colorizer2.setUniform("texture", textureMapTex);
    colorizer2.setUniform("overlay", dmap2.getOverlayMapTex());
    colorizer2.setUniform("paletteScale", Options::palette.scale(frame));
    colorizer2.setUniform("paletteOffset", Options::palette.offset(frame));
    colorizer2.setUniform("textureOpacity", Options::texture.image == "" ? 0.0 : Options::texture.opacity(frame));
    colorizer2.setUniform("textureScale", Options::texture.scale(frame));
    colorizer2.setUniform("textureOffset", Options::texture.offset(frame));
    colorizer2.apply();

    // 2. Illuminate
    if (Options::lighting.enable) {

        illuminator.setUniform("image", colorizer.getTexture());
        illuminator.setUniform("lightDir", lightVector(frame));
        illuminator.setUniform("normalRe", dmap1.getNormalReMapTex());
        illuminator.setUniform("normalIm", dmap1.getNormalImMapTex());
        illuminator.apply();

        illuminator2.setUniform("image", colorizer2.getTexture());
        illuminator2.setUniform("lightDir", lightVector(frame));
        illuminator2.setUniform("normalRe", dmap2.getNormalReMapTex());
        illuminator2.setUniform("normalIm", dmap2.getNormalImMapTex());
        illuminator2.apply();

        downscaler.setUniform("curr", illuminator.getTexture());
        downscaler.setUniform("next", illuminator2.getTexture());

    } else {

        downscaler.setUniform("curr", colorizer.getTexture());
        downscaler.setUniform("next", colorizer2.getTexture());
    }

    // 3. Scale down
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
    auto a = Options::lighting.alpha(frame) * 3.14159 / 180.0;
    auto b = Options::lighting.beta(frame) * 3.14159 / 180.0;
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
    if (Options::flags.verbose) {

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