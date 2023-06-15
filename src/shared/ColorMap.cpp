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

    if (width != w || height != h) {

        width = w;
        height = h;

        colorMap.resize(width * height);
        normalMap.resize(width * height);

        // Load color palette
        palette.load(opt.colors.palette);

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
    ProgressIndicator progress("Computing color map", map.height * map.width);

    resize(map.width, map.height);

    for (isize y = 0; y < height; y++) {
        for (isize x = 0; x < width; x++) {

            auto c = Coord(x,y);
            auto data = map.get(x, y);
            auto pos = y * width + x;

            //
            // Colorize image
            //

            if (map.isOutside(c)) {

                double sl = (double(data.iteration) - log2(data.lognorm)) + 4.0;
                sl *= .0025;
                // sl = 2.7 + sl * 30.0;
                sl *= 30.0;
                sl *= opt.colors.scale;

                colorMap[pos] = palette.interpolateABGR(sl);

            } else if (opt.debug.glitches && map.isGlitch(c)) {

                colorMap[pos] = 0xFF0000FF;

            } else if (opt.debug.rejected && map.isRejected(c)) {

                colorMap[pos] = 0xFF00FFFF;

            } else if (opt.debug.periodic && map.isPeriodic(c)) {

                colorMap[pos] = 0xFFFF00FF;

            } else {

                colorMap[pos] = 0xFF000000;
            }

            //
            // Colorize normal map
            //

            if (map.isOutside(c)) {

                auto r = (0.5 + (data.normal.re / 2.0)) * 255.0;
                auto g = (0.5 + (data.normal.im / 2.0)) * 255.0;
                auto b = 255.0;
                auto a = 255.0;

                normalMap[pos] = u8(a) << 24 | u8(b) << 16 | u8(g) << 8 | u8(r);

            } else if (opt.image.depth == 0) {

                normalMap[pos] = 0;
            }
        }

        if (opt.stop) throw UserInterruptException();
        progress.step(map.width);
    }

    colorMapTex.update((u8 *)colorMap.ptr);
    normalMapTex.update((u8 *)normalMap.ptr);

    progress.done();

    if (opt.flags.verbose) {

        auto path = palette.getPath();

        log::cout << log::vspace;
        log::cout << log::ralign("Map size: ");
        log::cout << width << " x " << height << log::endl;
        log::cout << log::ralign("Palette: ");
        log::cout << (path != "" ? path : "not specified") << log::endl;
        log::cout << log::vspace;
    }
}

}
