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
#include <tgmath.h>
#include <SFML/Graphics.hpp>

namespace dd {

Palette::Palette()
{
    static constexpr isize size = 2048;

    r.alloc(size);
    g.alloc(size);
    b.alloc(size);

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
        r[i] = u8(255.0 * (0.5 + 0.5 * cos(v + 0.0 - 2.7)));
        g[i] = u8(255.0 * (0.5 + 0.5 * cos(v + 0.6 - 2.7)));
        b[i] = u8(255.0 * (0.5 + 0.5 * cos(v + 1.0 - 2.7)));
    }
}

void
Palette::load(const string &path)
{
    sf::Image img;

    if (!fileExists(path)) {
        throw FileNotFoundError(path);
    }

    img.loadFromFile(path);
    auto size = img.getSize();

    r.alloc(size.x);
    g.alloc(size.x);
    b.alloc(size.x);

    for (unsigned x = 0; x < size.x; x++) {

        auto p = img.getPixel(x, 0);

        r[x] = p.r;
        g[x] = p.g;
        b[x] = p.b;
    }
}

u32
Palette::interpolateABGR(double value)
{
    auto size = r.size;

    auto scaled = value * size / (2 * 3.14159);
    auto frac = fmod(scaled, 1.0);

    auto r1 = double(r[(isize(scaled) + 0) % size]);
    auto g1 = double(g[(isize(scaled) + 0) % size]);
    auto b1 = double(b[(isize(scaled) + 0) % size]);
    auto r2 = double(r[(isize(scaled) + 1) % size]);
    auto g2 = double(g[(isize(scaled) + 1) % size]);
    auto b2 = double(b[(isize(scaled) + 1) % size]);
    auto mr = r1 + (r2 - r1) * frac;
    auto mg = g1 + (g2 - g1) * frac;
    auto mb = b1 + (b2 - b1) * frac;

    return  255 << 24 | u8(mb) << 16 | u8(mg) << 8 | u8(mr);
}

}
