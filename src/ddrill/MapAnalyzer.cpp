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

#include "MapAnalyzer.h"
#include "Options.h"
#include "ProgressIndicator.h"

namespace dd {

void
MapAnalyzer::analyze(const DrillMap &map)
{
    width = map.width;
    height = map.height;
    total = width * height;

    auto limit = Options::location.depth;

    {   ProgressIndicator progress("Analyzing drill map", total);

        for (isize y = 0; y < height; y++) {
            for (isize x = 0; x < width; x++) {

                auto i = y * width + x;

                // auto &entry = get(Coord(x,y));

                bool optspot = !!map.firstIterationMap[i];
                optspots.approximations += !!map.firstIterationMap[i];
                saved.approximations += map.firstIterationMap[i];
                saved.total += map.firstIterationMap[i];

                switch(map.resultMap[i]) {

                    case DR_UNPROCESSED:

                        spots.total++;
                        spots.unprocessed++;
                        break;

                    case DR_ESCAPED:

                        spots.total++;
                        spots.exterior++;
                        iterations.total += map.lastIterationMap[i];
                        iterations.exterior += map.lastIterationMap[i];
                        break;

                    case DR_MAX_DEPTH_REACHED:

                        spots.total++;
                        spots.interior++;
                        iterations.total += map.lastIterationMap[i];
                        iterations.interior += map.lastIterationMap[i];
                        assert(map.lastIterationMap[i] == limit);
                        break;

                    case DR_IN_BULB:

                        optspot = true;
                        spots.total++;
                        spots.interior++;
                        optspots.bulb++;
                        iterations.total += limit;
                        iterations.interior += limit;
                        saved.total += limit;
                        saved.bulb += limit;
                        break;

                    case DR_IN_CARDIOID:

                        optspot = true;
                        spots.total++;
                        spots.interior++;
                        optspots.cartioid++;
                        iterations.total += limit;
                        iterations.interior += limit;
                        saved.total += limit;
                        saved.cartioid += limit;
                        break;

                    case DR_PERIODIC:

                        optspot = true;
                        spots.total++;
                        spots.interior++;
                        optspots.periods++;
                        iterations.total += limit;
                        iterations.interior += limit;
                        saved.total += limit - map.lastIterationMap[i];
                        saved.periods += limit - map.lastIterationMap[i];
                        break;

                    case DR_ATTRACTED:

                        optspot = true;
                        spots.total++;
                        spots.interior++;
                        optspots.attractors++;
                        iterations.total += limit;
                        iterations.interior += limit;
                        saved.total += limit - map.lastIterationMap[i];
                        saved.attractors += limit - map.lastIterationMap[i];
                        break;

                    case DR_GLITCH:

                        spots.total++;
                        spots.glitches++;
                        break;
                }

                if (optspot) optspots.total++;
            }
            progress.step(width);
        }
    }
}

void
MapAnalyzer::print()
{
    auto format = [&](isize x) {

        usize digits = std::log10(iterations.total) + 1;

        string result = std::to_string(x);
        result = std::string(digits - std::min(digits, result.length()), ' ') + result;

        auto p = isize(std::round(10000.0 * double(x) / double(total)));
        auto q1 = std::to_string(p / 100);
        auto q2 = std::to_string(p % 100);
        q1 = std::string(3 - std::min(usize(3), q1.length()), ' ') + q1;
        q2 = q2 + std::string(2 - std::min(usize(2), q2.length()), '0');

        result += " (" + q1 + "." + q2 + " %)";

        return result;
    };

    log::cout << log::vspace;
    log::cout << "           Drill locations: " << log::endl;
    log::cout << log::endl;
    log::cout << log::ralign("Total: ");
    log::cout << format(spots.total) << log::endl;
    log::cout << log::ralign("Unprocessed: ");
    log::cout << format(spots.unprocessed) << log::endl;
    log::cout << log::ralign("Interior: ");
    log::cout << format(spots.interior) << log::endl;
    log::cout << log::ralign("Exterior: ");
    log::cout << format(spots.exterior) << log::endl;
    log::cout << log::ralign("Glitches: ");
    log::cout << format(spots.glitches) << log::endl;

    log::cout << log::vspace;
    log::cout << "           Locations with applied optimizations: " << log::endl;
    log::cout << log::endl;
    log::cout << log::ralign("Total: ");
    log::cout << format(optspots.total) << log::endl;
    log::cout << log::ralign("Main bulb filter: ");
    log::cout << format(optspots.bulb) << log::endl;
    log::cout << log::ralign("Cartioid filter: ");
    log::cout << format(optspots.cartioid) << log::endl;
    log::cout << log::ralign("Series approximation: ");
    log::cout << format(optspots.approximations) << log::endl;
    log::cout << log::ralign("Period detection: ");
    log::cout << format(optspots.periods) << log::endl;
    log::cout << log::ralign("Attractor detection: ");
    log::cout << format(optspots.attractors) << log::endl;

    total = iterations.total;

    log::cout << log::vspace;
    log::cout << "           Iteration counts: " << log::endl;
    log::cout << log::endl;
    log::cout << log::ralign("Total: ");
    log::cout << format(iterations.total) << log::endl;
    log::cout << log::ralign("Interior: ");
    log::cout << format(iterations.interior) << log::endl;
    log::cout << log::ralign("Exterior: ");
    log::cout << format(iterations.exterior) << log::endl;

    log::cout << log::vspace;
    log::cout << "           Skipped iterations: " << log::endl;
    log::cout << log::endl;
    log::cout << log::ralign("Total: ");
    log::cout << format(saved.total) << log::endl;
    log::cout << log::ralign("Main bulb filter: ");
    log::cout << format(saved.bulb) << log::endl;
    log::cout << log::ralign("Cartioid filter: ");
    log::cout << format(saved.cartioid) << log::endl;
    log::cout << log::ralign("Series approximation: ");
    log::cout << format(saved.approximations) << log::endl;
    log::cout << log::ralign("Period detection: ");
    log::cout << format(saved.periods) << log::endl;
    log::cout << log::ralign("Attractor detection: ");
    log::cout << format(saved.attractors) << log::endl;
    log::cout << log::endl;
}

}
