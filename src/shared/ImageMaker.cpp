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
ImageMaker::init()
{
    // Only proceed if initialization hasn't been done yet
    if (colorizer.getSize().x != 0) return;

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
    colorizer.init(Options::gpu.colorizer, mapDim);
    colorizer2.init(Options::gpu.colorizer, mapDim);
    illuminator.init(Options::gpu.illuminator, mapDim);
    illuminator2.init(Options::gpu.illuminator, mapDim);
    downscaler.init(Options::gpu.scaler, imageDim);
}

void
ImageMaker::draw(DrillMap &map)
{
    init();

    auto rgba = RgbColor(Options::palette.bgColor);

    {
        ProgressIndicator progress("Running GPU shaders");

        // 1. Colorize
        colorizer.setUniform("iter", map.getIterationMapTex());
        colorizer.setUniform("nitcnt", map.getNitcntMapTex());
        colorizer.setUniform("normalRe", map.getNormalReMapTex());
        colorizer.setUniform("normalIm", map.getNormalImMapTex());
        colorizer.setUniform("palette", paletteTex);
        colorizer.setUniform("paletteScale", Options::palette.scale());
        colorizer.setUniform("paletteOffset", Options::palette.offset());
        colorizer.setUniform("smooth", Options::palette.mode == ColoringMode::Smooth);
        colorizer.setUniform("bgcolor", sf::Glsl::Vec4(rgba.r, rgba.g, rgba.b, rgba.a));
        colorizer.setUniform("dist", map.distMapTex);
        colorizer.setUniform("distThreshold", Options::distance.enable ? Options::distance.threshold() : 0.0);
        colorizer.setUniform("texture", textureMapTex);
        colorizer.setUniform("textureOpacity", Options::texture.opacity());
        colorizer.setUniform("textureScale", Options::texture.scale());
        colorizer.setUniform("textureOffset", Options::texture.offset());
        colorizer.setUniform("overlay", map.getOverlayMapTex());
        colorizer.apply();

        // 2. Illuminate
        if (Options::lighting.enable) {

            illuminator.setUniform("image", colorizer.getTexture());
            illuminator.setUniform("lightDir", lightVector(0));
            illuminator.setUniform("normalRe", map.getNormalReMapTex());
            illuminator.setUniform("normalIm", map.getNormalImMapTex());
            illuminator.apply();

            downscaler.setUniform("curr", illuminator.getTexture());

        } else {

            downscaler.setUniform("curr", colorizer.getTexture());
        }

        // 3. Scale down
        downscaler.setUniform("size", sf::Vector2f(illuminator.getSize()));
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
ImageMaker::draw(DrillMap &map1, DrillMap &map2, isize frame, float zoom)
{
    init();

    auto rgba = RgbColor(Options::palette.bgColor);

    // 1. Colorize
    colorizer.setUniform("iter", map1.getIterationMapTex());
    colorizer.setUniform("nitcnt", map1.getNitcntMapTex());
    colorizer.setUniform("normalRe", map1.getNormalReMapTex());
    colorizer.setUniform("normalIm", map1.getNormalImMapTex());
    colorizer.setUniform("palette", paletteTex);
    colorizer.setUniform("paletteScale", Options::palette.scale(frame));
    colorizer.setUniform("paletteOffset", Options::palette.offset(frame));
    colorizer.setUniform("smooth", Options::palette.mode == ColoringMode::Smooth);
    colorizer.setUniform("bgcolor", sf::Glsl::Vec4(rgba.r, rgba.g, rgba.b, rgba.a));
    colorizer.setUniform("dist", map1.distMapTex);
    colorizer.setUniform("distThreshold", Options::distance.enable ? Options::distance.threshold(frame) : 0.0);
    colorizer.setUniform("texture", textureMapTex);
    colorizer.setUniform("textureOpacity", Options::texture.image == "" ? 0.0 : Options::texture.opacity(frame));
    colorizer.setUniform("textureScale", Options::texture.scale(frame));
    colorizer.setUniform("textureOffset", Options::texture.offset(frame));
    colorizer.setUniform("overlay", map1.getOverlayMapTex());
    colorizer.apply();

    colorizer2.setUniform("iter", map2.getIterationMapTex());
    colorizer2.setUniform("nitcnt", map2.getNitcntMapTex());
    colorizer2.setUniform("normalRe", map2.getNormalReMapTex());
    colorizer2.setUniform("normalIm", map2.getNormalImMapTex());
    colorizer2.setUniform("palette", paletteTex);
    colorizer2.setUniform("paletteScale", Options::palette.scale(frame));
    colorizer2.setUniform("paletteOffset", Options::palette.offset(frame));
    colorizer2.setUniform("smooth", Options::palette.mode == ColoringMode::Smooth);
    colorizer2.setUniform("bgcolor", sf::Glsl::Vec4(rgba.r, rgba.g, rgba.b, rgba.a));
    colorizer2.setUniform("dist", map2.distMapTex);
    colorizer2.setUniform("distThreshold", Options::distance.enable ? Options::distance.threshold(frame) : 0.0);
    colorizer2.setUniform("texture", textureMapTex);
    colorizer2.setUniform("textureOpacity", Options::texture.image == "" ? 0.0 : Options::texture.opacity(frame));
    colorizer2.setUniform("textureScale", Options::texture.scale(frame));
    colorizer2.setUniform("textureOffset", Options::texture.offset(frame));
    colorizer2.setUniform("overlay", map2.getOverlayMapTex());
    colorizer2.apply();

    // 2. Illuminate
    if (Options::lighting.enable) {

        illuminator.setUniform("image", colorizer.getTexture());
        illuminator.setUniform("lightDir", lightVector(frame));
        illuminator.setUniform("normalRe", map1.getNormalReMapTex());
        illuminator.setUniform("normalIm", map1.getNormalImMapTex());
        illuminator.apply();

        illuminator2.setUniform("image", colorizer2.getTexture());
        illuminator2.setUniform("lightDir", lightVector(frame));
        illuminator2.setUniform("normalRe", map2.getNormalReMapTex());
        illuminator2.setUniform("normalIm", map2.getNormalImMapTex());
        illuminator2.apply();

        downscaler.setUniform("curr", illuminator.getTexture());
        downscaler.setUniform("next", illuminator2.getTexture());

    } else {

        downscaler.setUniform("curr", colorizer.getTexture());
        downscaler.setUniform("next", colorizer2.getTexture());
    }

    // 3. Scale down
    downscaler.setUniform("size", sf::Vector2f(illuminator.getSize()));
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
