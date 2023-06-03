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

Colorizer::Colorizer(const Options &opt, const DrillMap &map) : opt(opt), map(map)
{
    // Allocate buffers
    image.alloc(map.width * map.height);
    normalMap.alloc(map.width * map.height);

    // Load color palette
    palette.load(opt.colors.palette);
}

Colorizer::~Colorizer()
{

}

void
Colorizer::colorize()
{
    ProgressIndicator progress("Colorizing", map.height * map.width);

    for (isize y = 0; y < map.height; y++) {
        
        for (isize x = 0; x < map.width; x++) {
            colorize(Coord(x,y));
        }

        if (opt.stop) throw UserInterruptException();
        progress.step(map.width);
    }
}

void
Colorizer::colorize(Coord c)
{
    auto data = map.get(c.x, c.y);
    auto pos = (map.height - 1 - c.y) * map.width + c.x;

    //
    // Colorize image
    //

    if (data.iteration == UINT32_MAX) {

        // Map to a black or to a debug color if the point is a glitch point
        image[pos] = opt.debug.glitches ? 0xFF0000FF : 0xFF000000;

    } else if (data.iteration == 0) {

        // Map to a black if the point belongs to the mandelbrot set
        image[pos] = 0xFF000000;

    /*
    } else if (opt.palette.mode == ColoringMode::Relief) {

        double a = 0.7;
        double b = 0.7;
        double h2 = 1.5;
        double t = data.normal.re * a + data.normal.im * b + h2;
        t = t / (1 + h2);
        if (t < 0) t = 0;
        t *= 255.0;

        image[pos] = 0xFF << 24 | u8(t) << 16 | u8(t) << 8 | u8(t);
    */

    } else {

        double sl = (double(data.iteration) - log2(data.lognorm)) + 4.0;
        sl *= .0025;
        // sl = 2.7 + sl * 30.0;
        sl *= 30.0;
        sl *= opt.colors.scale;

        image[pos] = palette.interpolateABGR(sl);
    }

    //
    // Colorize normal map
    //

    if (data.iteration == UINT32_MAX) {

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

void
Colorizer::save(const string &path, Format format)
{
    ProgressIndicator progress("Saving image data");

    auto prefix = stripSuffix(path);
    auto suffix = extractSuffix(path);
    auto width = (int)map.width;
    auto height = (int)map.height;

    sf::Image sfImage;
    sfImage.create(width, height, (u8 *)image.ptr);
    sfImage.saveToFile(path);

    if (opt.image.depth) {

        sfImage.create(width, height, (u8 *)normalMap.ptr);
        sfImage.saveToFile(prefix + "_nrm." + suffix);
    }
}

}
