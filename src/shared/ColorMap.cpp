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

void
ColorMap::resize(isize w, isize h)
{
    assert(w >= MIN_MAP_WIDTH && w <= MAX_MAP_WIDTH);
    assert(h >= MIN_MAP_HEIGHT && h <= MAX_MAP_HEIGHT);

    if (width != w || height != h) {

        width = w;
        height = h;

        iterMap.resize(width * height);
        overlayMap.resize(width * height);
        textureMap.resize(width * height);
        lognormMap.resize(width * height);
        normalReMap.resize(width * height);
        normalImMap.resize(width * height);

        // Load palette images
        palette.loadPaletteImage(opt.colors.palette);
        palette.loadTextureImage(opt.colors.texture);

        if (!iterMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create iteration map texture");
        }
        if (!overlayMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create overlay map texture");
        }
        if (!textureMapTex.loadFromImage(palette.texture)) {
            printf("Texture image faile to load\n");
            if (!textureMapTex.create(unsigned(width), unsigned(height))) {
                throw Exception("Can't create texture map texture");
            }
        }
        if (!lognormMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create lognorm map texture");
        }
        if (!normalReMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create normal(re) map texture");
        }
        if (!normalImMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create normal(im) map texture");
        }
        if (!paletteTex.loadFromImage(palette.palette)) {
            throw Exception("Can't create palette texture");
        }

    }
}

void
ColorMap::compute(const DrillMap &map)
{
    switch (opt.colors.mode) {

        case ColoringMode::Default:     compute <ColoringMode::Default> (map); break;
        // case ColoringMode::Textured:    compute <ColoringMode::Textured> (map); break;
    }
}

template <ColoringMode M> void
ColorMap::compute(const DrillMap &map)
{
    ProgressIndicator progress("Computing color map", map.height * map.width);

    // Resize to the size of the drill map
    resize(map.width, map.height);

    // Colorize all pixels
    for (isize y = 0; y < height; y++) {
        for (isize x = 0; x < width; x++) {

            auto c = Coord(x,y);
            auto data = map.get(x, y);
            auto pos = y * width + x;

            //
            // Colorize image
            //

            lognormMap[pos] = data.lognorm;
            iterMap[pos] = data.last;

            switch (map.get(c).result) {

                case DR_ESCAPED:

                    overlayMap[pos] = 0;
                    break;

                case DR_GLITCH:

                    overlayMap[pos] = opt.perturbation.color | 0xFF000000;
                    break;

                case DR_IN_BULB:
                case DR_IN_CARDIOID:

                    overlayMap[pos] = opt.areacheck.color | 0xFF000000;
                    break;

                case DR_PERIODIC:

                    overlayMap[pos] = opt.periodcheck.color | 0xFF000000;
                    break;

                case DR_ATTRACTED:

                    overlayMap[pos] = opt.attractorcheck.color | 0xFF000000;
                    break;

                default:

                    overlayMap[pos] = GpuColor::black | 0xFF000000;
                    break;
            }


            //
            // Generate normal map
            //

            if (opt.image.depth == 1 && map.get(c).result == DR_ESCAPED) {

                normalReMap[pos] = float(data.normal.re);
                normalImMap[pos] = float(data.normal.im);

            } else {

                normalReMap[pos] = 0.0;
                normalImMap[pos] = 0.0;
            }
        }

        if (opt.stop) throw UserInterruptException();
        progress.step(map.width);
    }

    iterMapTex.update((u8 *)iterMap.data());
    overlayMapTex.update((u8 *)overlayMap.data());
    lognormMapTex.update((u8 *)lognormMap.data());
    normalReMapTex.update((u8 *)normalReMap.data());
    normalImMapTex.update((u8 *)normalImMap.data());
    progress.done();

    if (opt.flags.verbose) {

        log::cout << log::vspace;
        log::cout << log::ralign("Map size: ");
        log::cout << width << " x " << height << log::endl;
        log::cout << log::ralign("Palette: ");
        log::cout << (opt.colors.palette != "" ? opt.colors.palette : "not specified") << log::endl;
        log::cout << log::ralign("Texture: ");
        log::cout << (opt.colors.texture != "" ? opt.colors.texture : "not specified") << log::endl;
        log::cout << log::vspace;
    }
}

}
