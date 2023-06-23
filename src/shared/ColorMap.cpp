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

        colorMap.resize(width * height);
        normalMap.resize(width * height);

        // Load palette images
        palette.loadPaletteImage(opt.colors.palette);
        palette.loadTextureImage(opt.colors.texture);

        // Create textures
        if (!colorMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create color map texture");
        }
        if (!normalMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create normal map texture");
        }
    }
}

void
ColorMap::compute(const DrillMap &map)
{
    switch (opt.colors.mode) {

        case ColoringMode::Default:     compute <ColoringMode::Default> (map); break;
        case ColoringMode::Textured:    compute <ColoringMode::Textured> (map); break;
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

            double sl;

            switch (map.get(c).result) {

                case DR_ESCAPED:

                    if constexpr (M == ColoringMode::Default) {

                        sl = (double(data.last) - log2(data.lognorm)) + 4.0;
                        sl *= .0025;
                        // sl = 2.7 + sl * 30.0;
                        sl *= 30.0;
                        sl *= opt.colors.scale;

                        colorMap[pos] = palette.interpolateABGR(sl);
                    }

                    if constexpr (M == ColoringMode::Textured) {

                        colorMap[pos] = palette.readTextureImage(data);
                    }
                    break;

                case DR_GLITCH:

                    colorMap[pos] = opt.perturbation.color;
                    break;

                case DR_IN_BULB:
                case DR_IN_CARDIOID:

                    colorMap[pos] = opt.areacheck.color;
                    break;

                case DR_PERIODIC:

                    colorMap[pos] = opt.periodcheck.color;
                    break;

                case DR_ATTRACTED:

                    colorMap[pos] = opt.attractorcheck.color;
                    break;

                default:

                    colorMap[pos] = 0xFF000000;
                    break;
            }


            //
            // Generate normal map
            //

            if (opt.image.depth == 1 && map.get(c).result == DR_ESCAPED) {

                auto r = (0.5 + (data.normal.re / 2.0)) * 255.0;
                auto g = (0.5 + (data.normal.im / 2.0)) * 255.0;
                auto b = 255.0;
                auto a = 255.0;

                normalMap[pos] = u8(a) << 24 | u8(b) << 16 | u8(g) << 8 | u8(r);

            } else {

                normalMap[pos] = 0;
            }
        }

        if (opt.stop) throw UserInterruptException();
        progress.step(map.width);
    }

    colorMapTex.update((u8 *)colorMap.data());
    normalMapTex.update((u8 *)normalMap.data());

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
