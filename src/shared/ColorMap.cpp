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

#include "ColorMap.h"
#include "Filter.h"
#include "Coord.h"
#include "DrillMap.h"
#include "Exception.h"
#include "IO.h"
#include "Logger.h"
#include "Options.h"
#include "ProgressIndicator.h"

#include <bit>
#include <SFML/Graphics.hpp>

namespace dd {

ColorMap::~ColorMap()
{

}

void
ColorMap::resize(isize w, isize h)
{
    assert(w >= MIN_MAP_WIDTH && w <= MAX_MAP_WIDTH);
    assert(h >= MIN_MAP_HEIGHT && h <= MAX_MAP_HEIGHT);

    width = w;
    height = h;

    colorMap.resize(width * height);
    normalMap.resize(width * height);

    // Load color palette
    palette.load(opt.colors.palette);

    // Create textures
    auto mapDim = sf::Vector2u(unsigned(opt.drillmap.width), unsigned(opt.drillmap.height));
    auto imageDim = sf::Vector2u(unsigned(opt.image.width), unsigned(opt.image.height));
    initTexture(colorMapTex, sourceRect, mapDim);
    initTexture(normalMapTex, mapDim);
    initRenderTexture(finalTex, targetRect, imageDim);

    // Load shaders
    initShader(scaler, opt.image.scaler);
}

/*
void
ColorMap::init()
{
    if (colorMap.size != 0) return;

    auto mapDim = sf::Vector2u(unsigned(opt.drillmap.width), unsigned(opt.drillmap.height));
    auto imageDim = sf::Vector2u(unsigned(opt.image.width), unsigned(opt.image.height));

    // Allocate buffers
    colorMap.alloc(mapDim.x * mapDim.y);
    normalMap.alloc(mapDim.x * mapDim.y);

    // Load color palette
    palette.load(opt.colors.palette);

    // Create textures
    initTexture(colorMapTex, sourceRect, mapDim);
    initTexture(normalMapTex, mapDim);
    
    initRenderTexture(finalTex, targetRect, imageDim);

    // Load shaders
    initShader(scaler, opt.image.scaler);
}
*/

void
ColorMap::initTexture(sf::Texture &tex, sf::Vector2u size)
{
    if (!tex.create(size.x, size.y)) {
        throw Exception("Can't create texture");
    }
    tex.setSmooth(false);
}

void
ColorMap::initTexture(sf::Texture &tex, sf::RectangleShape &rect, sf::Vector2u size)
{
    initTexture(tex, size);
    rect.setSize(sf::Vector2f(size.x, size.y));
    rect.setTexture(&tex);
}

void
ColorMap::initRenderTexture(sf::RenderTexture &tex, sf::Vector2u size)
{
    if (!tex.create(size.x, size.y)) {
        throw Exception("Can't create render texture");
    }
    tex.setSmooth(false);
}

void
ColorMap::initRenderTexture(sf::RenderTexture &tex, sf::RectangleShape &rect, sf::Vector2u size)
{
    initRenderTexture(tex, size);
    rect.setSize(sf::Vector2f(size.x, size.y));
    rect.setTexture(&tex.getTexture());
}

void
ColorMap::initShader(sf::Shader &shader, const string &name)
{
    auto path = opt.assets.findAsset(name, Format::GLSL);

    if (path == "") {
        throw Exception("Can't load fragment shader '" + name + "'");
    }
    if (!shader.loadFromFile(path, sf::Shader::Fragment)) {
        throw Exception("Can't load fragment shader '" + path.string() + "'");
    }
}

void
ColorMap::update(const DrillMap &map)
{
    ProgressIndicator progress("Colorizing", map.height * map.width);

    assert(map.width == opt.drillmap.width);
    assert(map.height == opt.drillmap.height);

    resize(map.width, map.height);

    for (isize y = 0; y < map.height; y++) {
        for (isize x = 0; x < map.width; x++) {

            auto data = map.get(x, y);
            auto pos = (map.height - 1 - y) * map.width + x;

            //
            // Colorize image
            //

            if (data.iteration == UINT32_MAX) {

                // Map to a black or to a debug color if the point is a glitch point
                colorMap[pos] = opt.debug.glitches ? 0xFF0000FF : 0xFF000000;

            } else if (data.iteration == 0) {

                // Map to a black if the point belongs to the mandelbrot set
                colorMap[pos] = 0xFF000000;

            } else {

                double sl = (double(data.iteration) - log2(data.lognorm)) + 4.0;
                sl *= .0025;
                // sl = 2.7 + sl * 30.0;
                sl *= 30.0;
                sl *= opt.colors.scale;

                colorMap[pos] = palette.interpolateABGR(sl);
            }

            //
            // Colorize normal map
            //

            if (opt.image.depth == 0) {

                normalMap[pos] = 0;

            } else if (data.iteration == UINT32_MAX) {

                // Map to zero if the point is a glitch point
                normalMap[pos] = 0;

            } else if (data.iteration == 0) {

                // Map to zero if the point belongs to the mandelbrot set
                normalMap[pos] = 0;

            } else {

                auto r = (0.5 + (data.normal.re / 2.0)) * 255.0;
                auto g = (0.5 + (data.normal.im / 2.0)) * 255.0;
                auto b = 255.0;
                auto a = 255.0;

                normalMap[pos] = u8(a) << 24 | u8(b) << 16 | u8(g) << 8 | u8(r);
            }
        }

        if (opt.stop) throw UserInterruptException();
        progress.step(map.width);
    }

    colorMapTex.update((u8 *)colorMap.ptr);
    normalMapTex.update((u8 *)normalMap.ptr);

    computeImage();
}

sf::Image &
ColorMap::computeImage()
{
    // ProgressIndicator progress("Composing final image");

    // Setup uniforms
    auto a = opt.colors.alpha * 3.14159 / 180.0;
    auto b = opt.colors.beta * 3.14159 / 180.0;

    auto z = std::sin(b);
    auto l = std::cos(b);
    auto y = std::sin(a) * l;
    auto x = std::cos(a) * l;

    scaler.setUniform("size", sf::Vector2f(colorMapTex.getSize()));
    scaler.setUniform("image", colorMapTex);
    scaler.setUniform("normal", normalMapTex);
    scaler.setUniform("lightDir", sf::Vector3f(x, y, z));
    finalTex.draw(targetRect, &scaler);
    finalTex.display();

    // Read back image data
    final = finalTex.getTexture().copyToImage();
    return final;
}

void
ColorMap::save(const string &path, Format format)
{
    ProgressIndicator progress("Saving image data");

    auto prefix = stripSuffix(path);
    auto suffix = extractSuffix(path);
    auto width = (int)opt.drillmap.width;
    auto height = (int)opt.drillmap.height;

    final.saveToFile(path);

    if (opt.exports.colorMap) {

        sf::Image img;
        img.create(width, height, (u8 *)colorMap.ptr);
        img.saveToFile(prefix + "_raw." + suffix);
    }

    if (opt.exports.normalMap) {

        sf::Image img;
        img.create(width, height, (u8 *)normalMap.ptr);
        img.saveToFile(prefix + "_nrm." + suffix);
    }
}

}
