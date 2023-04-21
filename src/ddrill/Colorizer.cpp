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

Colorizer::Colorizer(const Options &o, const DrillMap &m) : opt(o), map(m)
{
    auto values = opt.palette.values;

    if (std::count(values.cbegin(), values.cend(), ' ') != 0) {

        // Custom palette
        scheme = ColorScheme::Custom;

        std::stringstream stream(values);
        u32 value;

        while(stream >> value) {

            auto r = u8(value & 0xFF);
            auto g = u8((value >> 8) & 0xFF);
            auto b = u8((value >> 16) & 0xFF);

            colors.push_back(u32(0xFF << 24 | b << 16 | g << 8 | r));
        }

    } else {

        // Default palette
        scheme = ColorScheme::Default;
    }

    // Allocate image data
    image.alloc(map.width * map.height);
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

    // Map to a black if the point belongs to the mandelbrot set
    if (data.iteration == 0) {

        image[c.y * map.width + c.x] = 255 << 24;
        return;
    }

    switch (scheme) {

        case ColorScheme::Custom:
        {
            isize index = (isize)((data.iteration - log2(data.lognorm)) * 5);
            image[c.y * map.width + c.x] = colors[index % colors.size()];
            break;
        }
        case ColorScheme::Default:
        {
            // Adapted from https://www.shadertoy.com/view/tllSWj
            float sl = (float(data.iteration) - log2(data.lognorm)) + 4.0;
            sl *= .0025;

            float r = 0.5 + 0.5 * cos(2.7 + sl * 30.0 + 0.0);
            float g = 0.5 + 0.5 * cos(2.7 + sl * 30.0 + 0.6);
            float b = 0.5 + 0.5 * cos(2.7 + sl * 30.0 + 1.0);
            auto rr = u8(r * 255.0);
            auto gg = u8(g * 255.0);
            auto bb = u8(b * 255.0);

            image[c.y * map.width + c.x] = 255 << 24 | bb << 16 | gg << 8 | rr;
            break;
        }
    }
}

void
Colorizer::save(const string &path, Format format)
{
    ProgressIndicator progress("Saving image data");

    auto width = (int)map.width;
    auto height = (int)map.height;

    sf::Image sfImage;
    sfImage.create(width, height, (u8 *)image.ptr);
    sfImage.saveToFile(path);
}

}
