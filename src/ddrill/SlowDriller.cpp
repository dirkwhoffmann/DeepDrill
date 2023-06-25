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

#include "SlowDriller.h"
#include "ProgressIndicator.h"

namespace dd {

void
SlowDriller::drill()
{
    std::vector<Coord> remaining;

    auto width = opt.drillmap.width;
    auto height = opt.drillmap.height;

    for (isize y = 0; y < height; y++) {
        for (isize x = 0; x < width; x++) {
            remaining.push_back(Coord(x,y));
        }
    }

    drill(remaining);
}

void
SlowDriller::drill(const std::vector<Coord> &remaining)
{
    ProgressIndicator progress("Running the legacy driller", remaining.size());

    for (unsigned int i = 0; i < remaining.size(); i++) {

        drill(remaining[i]);

        if (opt.stop) throw UserInterruptException();
        progress.step(1);
    }
}

void
SlowDriller::drill(const Coord &point)
{
    // auto x0 = ExtendedComplex(point.translate(opt));
    auto x0 = ExtendedComplex(map.translate(point));
    auto xn = x0;

    auto d0 = ExtendedComplex(1, 0);
    auto dn = d0;

    isize limit = opt.location.depth;
    isize iteration = 0;

    // Enter the main loop
    while (++iteration < limit) {

        dn *= xn * 2.0;
        dn += d0;
        dn.reduce();

        xn *= xn;
        xn += x0;
        xn.reduce();

        auto norm = xn.norm().asDouble();

        // Perform the escape check
        if (norm >= 256) {

            // This point is outside the Mandelbrot set
            auto u = xn / dn;
            u.normalize();

            map.set(point, MapEntry {
                .result     = DR_ESCAPED,
                .last       = (i32)iteration,
                .lognorm    = (float)::log(norm),
                .derivative = StandardComplex(dn),
                .normal     = StandardComplex(u) } );
            return;
        }
    }

    // This point is inside the Mandelbrot set
    map.set(point, MapEntry {
        .result     = DR_MAX_DEPTH_REACHED,
        .last       = (i32)iteration } );
}

}
