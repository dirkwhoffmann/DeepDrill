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
#include <SFML/Graphics.hpp>

namespace dd {

Palette::Palette()
{
    const isize size = 2048;

    abgr.alloc(size);
    r.alloc(size);
    g.alloc(size);
    b.alloc(size);

    // Create default palette
    for (isize i = 0; i < abgr.size; i++) {

        double v = double(i) / abgr.size * 2 * 3.14159;

        double rrr = 0.5 + 0.5 * cos(2.7 + v * 30.0 + 0.0);
        double ggg = 0.5 + 0.5 * cos(2.7 + v * 30.0 + 0.6);
        double bbb = 0.5 + 0.5 * cos(2.7 + v * 30.0 + 1.0);
        auto rr = u8(rrr * 255.0);
        auto gg = u8(ggg * 255.0);
        auto bb = u8(bbb * 255.0);

        r[i] = rr;
        g[i] = gg;
        b[i] = bb;
        abgr[i] = 255 << 24 | bb << 16 | gg << 8 | rr;
    }
}

void
Palette::load(const string &image)
{

}

u32
Palette::interpolateABGR(double value)
{
    auto scaled = value * abgr.size / (2 * 3.14159);
    auto frac = fmod(scaled, 1.0);

    auto r1 = double(r[(isize(scaled) + 0) % abgr.size]);
    auto g1 = double(g[(isize(scaled) + 0) % abgr.size]);
    auto b1 = double(b[(isize(scaled) + 0) % abgr.size]);
    auto r2 = double(r[(isize(scaled) + 1) % abgr.size]);
    auto g2 = double(g[(isize(scaled) + 1) % abgr.size]);
    auto b2 = double(b[(isize(scaled) + 1) % abgr.size]);
    auto mr = r1 + (r2 - r1) * frac;
    auto mg = g1 + (g2 - g1) * frac;
    auto mb = b1 + (b2 - b1) * frac;

    return  255 << 24 | u8(mb) << 16 | u8(mg) << 8 | u8(mr);
}

}
