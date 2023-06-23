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

#include "Palette.h"
#include "Exception.h"
#include "IO.h"
#include "Options.h"
#include "DrillMap.h"
#include <tgmath.h>
#include <SFML/Graphics.hpp>

namespace dd {

Palette::Palette(const Options &opt) : opt(opt)
{
    static constexpr isize size = 2048;

    r.resize(size);
    g.resize(size);
    b.resize(size);

    /* Create default palette
     *
     * The formula is an adaption from https://www.shadertoy.com/view/tllSWj
     * It was used in an GLSL shader in the following form:
     *
     *     vec4 mapColor(float mcol) {
     *        return vec4(0.5 + 0.5*cos(2.7+mcol*30.0 + vec3(0.0,.6,1.0)),1.0);
     *     }
     */

    for (isize i = 0; i < size; i++) {

        double v = double(i) / size * 2 * 3.14159;
        r[i] = 255.0 * (0.5 + 0.5 * cos(v + 0.0 - 2.7));
        g[i] = 255.0 * (0.5 + 0.5 * cos(v + 0.6 - 2.7));
        b[i] = 255.0 * (0.5 + 0.5 * cos(v + 1.0 - 2.7));
    }
}

void
Palette::loadPaletteImage(const fs::path &path)
{
    sf::Image img;

    if (path != "" && img.loadFromFile(path)) {

        auto size = img.getSize();

        r.resize(size.x);
        g.resize(size.x);
        b.resize(size.x);

        for (unsigned x = 0; x < size.x; x++) {

            auto p = img.getPixel(x, 0);

            r[x] = p.r;
            g[x] = p.g;
            b[x] = p.b;
        }
    }
}

void
Palette::loadTextureImage(const fs::path &path)
{
    if (path == "") return;

    if (texture.loadFromFile(path)) {
        // printf("Loaded texture image %s\n", path.c_str());
    } else {
        printf("Failed to load texture image %s\n", path.c_str());
    }
}

u32
Palette::interpolateABGR(double value) const
{
    auto size = r.size();

    auto scaled = value * size / (2 * 3.14159);
    auto frac = fmod(scaled, 1.0);

    auto r1 = r[(isize(scaled) + 0) % size];
    auto g1 = g[(isize(scaled) + 0) % size];
    auto b1 = b[(isize(scaled) + 0) % size];
    auto r2 = r[(isize(scaled) + 1) % size];
    auto g2 = g[(isize(scaled) + 1) % size];
    auto b2 = b[(isize(scaled) + 1) % size];
    auto mr = r1 + (r2 - r1) * frac;
    auto mg = g1 + (g2 - g1) * frac;
    auto mb = b1 + (b2 - b1) * frac;

    return  255 << 24 | u8(mb) << 16 | u8(mg) << 8 | u8(mr);
}

u32
Palette::readTextureImage(struct MapEntry &entry) const
{
    const double PI = 3.141592653589793238;

    auto sl = ((double(entry.last) - log2(entry.lognorm)) + 4.0) * 0.075;
    sl *= opt.colors.scale;

    auto size = texture.getSize();

    auto arg = (entry.normal.arg() + PI) / (2 * PI);
    auto px = isize(arg * size.x * 5.0) % size.x;
    auto py = isize(sl * size.y * 5.0) % size.y;
    if (py < 0) py += size.y;
    if (px < 0) px += size.x;

    auto color = texture.getPixel(unsigned(px), unsigned(py));
    return 255 << 24 | color.b << 16 | color.g << 8 | color.r;
}

/*
u32
Palette::readTextureImage(double re, double im, double nre, double nim, double x, double y, double sl)
{
    const double PI = 3.141592653589793238;

    auto size = texture.getSize();

    auto scaled = sl * size.y * 5.0; //  / (2 * 3.14159);
    auto py = isize(scaled) % size.y;
    // auto arg = (StandardComplex(x,y).arg() + PI) / (2 * PI);
    auto arg = (StandardComplex(nre,nim).arg() + PI) / (2 * PI);

    auto px = isize(arg * size.x * 5.0) % size.x; //  / (2 * 3.14159);
    if (py < 0) py += size.y;
    if (px < 0) px += size.x;

    auto color = texture.getPixel(unsigned(px), unsigned(py));

    return 255 << 24 | color.b << 16 | color.g << 8 | color.r;
}
*/

}
