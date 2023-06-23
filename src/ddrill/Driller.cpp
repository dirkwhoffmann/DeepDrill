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

#include "Driller.h"
#include "ColorMap.h"
#include "Exception.h"
#include "Logger.h"
#include "Options.h"
#include "ProgressIndicator.h"

#include <random>

namespace dd {

Driller::Driller(const Options &o, DrillMap &m) : opt(o), map(m)
{

}

void
Driller::drill()
{    
    std::vector<Coord> remaining;
    std::vector<Coord> glitches;

    auto width = opt.drillmap.width;
    auto height = opt.drillmap.height;

    // Determine the number of tolerated glitched pixels
    isize threshold = width * height * opt.perturbation.badpixels;

    if (opt.flags.verbose) {

        assert(map.center.re.get_prec() == map.center.im.get_prec());
        assert(map.ul.re.get_prec() == map.ul.im.get_prec());
        assert(map.lr.re.get_prec() == map.lr.im.get_prec());

        log::cout << log::vspace;
        log::cout << log::ralign("Center: ");
        log::cout << (map.center.re >= 0.0 ? " " : "") << map.center;
        log::cout << " (" << map.center.re.get_prec() << " bit)" << log::endl;
        log::cout << log::ralign("Upper left: ");
        log::cout << (map.ul.re >= 0.0 ? " " : "") << map.ul;
        log::cout << " (" << map.ul.re.get_prec() << " bit)" << log::endl;
        log::cout << log::ralign("Lower right: ");
        log::cout << (map.lr.re >= 0.0 ? " " : "") << map.lr;
        log::cout << " (" << map.lr.re.get_prec() << " bit)" << log::endl;
        log::cout << log::ralign("Magnification: ");
        log::cout << opt.location.zoom << log::endl;
        log::cout << log::ralign("Drill depth: ");
        log::cout << opt.location.depth << log::endl;
        log::cout << log::endl;
        log::cout << log::ralign("Map size: ");
        log::cout << opt.drillmap.width << " x " << opt.drillmap.height;
        log::cout << (opt.drillmap.depth ? " (3D)" : " (2D)") << log::endl;
        log::cout << log::ralign("Image size: ");
        log::cout << opt.image.width << " x " << opt.image.height;
        log::cout << (opt.image.depth ? " (3D)" : " (2D)") << log::endl;
        log::cout << log::endl;
        log::cout << log::ralign("Perturbation: ");
        log::cout << opt.perturbation.enable << log::endl;
        log::cout << log::ralign("Series approximation: ");
        log::cout << opt.approximation.enable << log::endl;
        log::cout << log::ralign("Area checking: ");
        log::cout << opt.areacheck.enable << log::endl;
        log::cout << log::ralign("Period checking: ");
        log::cout << opt.periodcheck.enable << log::endl;
        log::cout << log::ralign("Attractor checking: ");
        log::cout << opt.attractorcheck.enable << log::endl;
        log::cout << log::vspace;
    }

    // Collect all pixel coordinates to be drilled at
    collectCoordinates(remaining);

    // Enter the main loop
    for (isize round = 1; round <= opt.perturbation.rounds; round++) {

        // Exit once enough pixels have been computed
        if ((isize)remaining.size() <= threshold) break;

        log::cout << log::vspace;
        log::cout << "Round " << round;
        if (opt.flags.verbose) log::cout << " / " << opt.perturbation.rounds;
        log::cout << ": ";
        log::cout << remaining.size() << " points remaining" << log::endl << log::endl;

        // Select a reference point
        ref = pickReference(glitches);
        
        // Drill the reference point
        drill(ref);
        
        if (opt.flags.verbose) {

            log::cout << log::vspace;
            log::cout << log::ralign("Reference point: ");
            log::cout << ref.coord << log::endl;
            log::cout << log::ralign("Perturbation tolerance: ");
            log::cout << opt.perturbation.tolerance << log::endl;
            log::cout << log::ralign("Maximum depth: ");
            log::cout << opt.location.depth << log::endl;
            log::cout << log::ralign("Actual depth: ");
            log::cout << ref.xn.size() << log::endl;
            log::cout << log::vspace;
        }
        
        // If series approximation is enabled...
        if (opt.approximation.enable) {
        
            // Compute the coefficients
            approximator.compute(ref, opt.approximation.coefficients, opt.location.depth);

            // Pick the probe points
            pickProbePoints(probePoints);
            
            // Drill the probe points
            ref.skipped = drillProbePoints(probePoints);

            // Make sure that at least one iteration of the main loop is executed
            if (ref.skipped == isize(ref.xn.size())) ref.skipped -= 2;
            if (ref.skipped < 0) ref.skipped = 0;

            if (opt.flags.verbose) {

                log::cout << log::vspace;
                log::cout << log::ralign("Skippable iterations: ");
                log::cout << ref.skipped << log::endl;
                log::cout << log::vspace;
            }
        }

        // Drill the remaining pixels
        drill(remaining, glitches);
        remaining = glitches;
        
        if (opt.flags.verbose) {
            
            log::cout << log::vspace;
            log::cout << log::ralign("Glitches: ");
            log::cout << glitches.size() << log::endl;
            log::cout << log::vspace;
        }        
    }

    log::cout << log::vspace;
    log::cout << "All rounds completed: ";
    log::cout << (isize)remaining.size() << " unresolved";
    log::cout << log::endl << log::endl;
}

void
Driller::collectCoordinates(std::vector<dd::Coord> &remaining)
{
    /* This function collects all drill coordinates while filtering out all
     * coordinates that belong to the main bulb or the cardioid (for which
     * simple inside/outside tests exist).
     *
     * Strategy: The map area is superimposed with a grid. For each grid
     * coordiate, bulb and cardioid checking is performed. If at least one
     * pixel belongs to the bulb or the cardioid, the check is repeated for all
     * pixels (which may take some seconds depending on the map size). If the
     * test is negative for all points of the mesh, it is assumed that the
     * drill area does not intercept the bulb or the cardioid. In this case,
     * all locations need to be drilled.
     */

    auto width = opt.drillmap.width;
    auto height = opt.drillmap.height;

    // If area checking if disabled, drill everywhere
    if (!opt.areacheck.enable) {

        for (isize y = 0; y < height; y++) {
            for (isize x = 0; x < width; x++) {
                remaining.push_back(Coord(x,y));
            }
        }
        return;
    }

    // Perform the area check
    ProgressIndicator progress("Running the area check", width * height);

    // Superimpose the drill map with a mesh
    std::vector<Coord> mesh; map.getMesh(8, 8, mesh);

    // Test if at least one mesh point belongs to the bulb or the cardioid
    bool hit = false;
    for (const auto &it : mesh) {

        // auto c = it.translate(opt);
        auto c = map.translate(it);
        hit |= c.inCardioid();
        hit |= c.inMainBulb();
    }

    // Collect all drill coordinates
    for (isize y = 0; y < height; y++) {
        for (isize x = 0; x < width; x++) {

            if (hit) {

                auto c = map.translate(Coord(x,y));

                if (c.inCardioid()) {

                    map.set(x, y, MapEntry { .result = DR_IN_CARDIOID });
                    continue;
                }
                if (c.inMainBulb()) {

                    map.set(x, y, MapEntry { .result = DR_IN_BULB });
                    continue;
                }
            }
            remaining.push_back(Coord(x,y));
        }
        progress.step(width);
    }
}

ReferencePoint
Driller::pickReference(const std::vector<Coord> &glitches)
{
    // Current strategy: In the first round, the center is used as reference
    // point. In all other rounds, the reference point is selected randomly
    // among all glitch points.

    bool firstRound = glitches.empty();

    if (firstRound) {

        auto coord = Coord(map.width / 2, map.height / 2);
        return ReferencePoint(coord, map.translate(coord));

    } else {
        
        auto coord = glitches[rand() % glitches.size()];
        return ReferencePoint(coord, map.translate(coord));
    }
}

void
Driller::pickProbePoints(std::vector<Coord> &probes)
{
    // Current strategy: The image canvas is superimposed with an equidistant
    // mesh. The density of the mesh is controlled by the 'sampling' parameter.
    // The minimum value is 2 which produces a mesh that comprises the four
    // corner points.
    
    static const isize sampling = 2;

    probes.clear();
    map.getMesh(sampling, sampling, probes);
}

void
Driller::drill(ReferencePoint &r)
{
    ProgressIndicator progress("Computing reference orbit", opt.location.depth);

    PrecisionComplex z = r.location;

    PrecisionComplex d0 { 1.0, 0.0 };
    PrecisionComplex dn = d0;

    r.xn.push_back(ReferenceIteration(z, opt.perturbation.tolerance));
        
    for (isize i = 1; i < opt.location.depth; i++) {

        // Compute derivative
        dn *= z * 2.0;
        dn += d0;
        
        // Compute next number
        z *= z;
        z += r.location;
        
        r.xn.push_back(ReferenceIteration(z, dn, opt.perturbation.tolerance));

        double norm = StandardComplex(z).norm();

        // Perform the escape check
        if (norm >= 256) {

            auto nv = z / dn;
            nv.normalize();

            r.escaped = true;
            map.set(r.coord, MapEntry {
                .result     = DR_ESCAPED,
                .last       = (i32)i,
                .lognorm    = (float)::log(norm),
                .derivative = StandardComplex(dn),
                .normal     = StandardComplex(nv) } );
            return;
        }
        
        // Update the progress counter
        if (i % 1024 == 0) {
            if (opt.stop) throw UserInterruptException();
            progress.step(1024);
        }
    }

    // This point is inside the Mandelbrot set
    map.set(r.coord, MapEntry {
        .result     = DR_MAX_DEPTH_REACHED,
        .last       = (i32)opt.location.depth });
}

isize
Driller::drillProbePoints(std::vector<Coord> &probes)
{
    ProgressIndicator progress("Checking probe points", probes.size());
    
    isize minValid = opt.location.depth - 1;
    
    for (auto &probe : probes) {

        isize valid = drillProbePoint(probe);
        minValid = std::min(minValid, valid);

        if (opt.stop) throw UserInterruptException();
        progress.step();
    }
    
    return minValid;
}

isize
Driller::drillProbePoint(Coord &probe)
{
    ExtendedComplex d0 = map.distance(probe, ref.coord);
    ExtendedComplex dn = d0;

    isize iteration = 0;
    auto tolerance = ExtendedDouble(opt.approximation.tolerance);
    
    // The depth of the reference point limits how deep we can drill
    isize limit = ref.xn.size();
    
    // Enter the main loop
    while (++iteration < limit) {

        dn *= ref.xn[iteration - 1].extended2 + dn;
        dn += d0;
        dn.reduce();
                
        auto approx = approximator.evaluate(probe, d0, iteration);
        auto error = (approx - dn).norm() / dn.norm();
        error.reduce();

        if (error > tolerance) {
            return iteration < 4 ? 0 : iteration - 4;
        }
    }
    return limit;
}

void
Driller::drill(const std::vector<Coord> &remaining, std::vector<Coord> &glitches)
{
    ProgressIndicator progress("Computing delta orbits", remaining.size());
    
    glitches.clear();
    for (unsigned int i = 0; i < remaining.size(); i++) {
        
        drill(remaining[i], glitches);
        
        if (opt.stop) throw UserInterruptException();
        progress.step(1);
    }
}

void
Driller::drill(const Coord &point, std::vector<Coord> &glitchPoints)
{
    // If this point is the reference point, there is nothing to do
    if (point == ref.coord) return;

    // The depth of the reference point limits how deep we can drill
    isize limit = ref.xn.size();

    // Determine the iteration to start with
    isize iteration = ref.skipped;

    // Setup orbit parameters
    ExtendedComplex d0 = map.distance(point, ref.coord);
    ExtendedComplex dn = d0;

    // Setup derivation parameters (df/dc)
    ExtendedComplex dd0 = ExtendedComplex(1.0, 0.0);
    ExtendedComplex ddn = dd0;

    // Setup derivation parameters (df/dz)
    ExtendedComplex derz0 = ExtendedComplex(1.0, 0.0);
    ExtendedComplex derzn = derz0;

    // Prepare for period checking
    ExtendedComplex p = dn;
    isize nextUpdate = iteration + 16;

    // Perform series approximation if applicable
    if (ref.skipped) {

        dn = approximator.evaluate(point, d0, iteration);
        dn.reduce();
        ddn = approximator.evaluateDerivate(point, d0, iteration);
        ddn.reduce();
    }

    //
    // Main loop
    //

    while (++iteration < limit) {

        ddn *= ref.xn[iteration - 1].extended2 + (dn * 2.0);
        ddn += dd0;
        ddn.reduce();

        derzn *= ref.xn[iteration - 1].extended2 + (dn * 2.0);
        derzn.reduce();

        dn *= ref.xn[iteration - 1].extended2 + dn;
        dn += d0;
        dn.reduce();

        auto zn = ref.xn[iteration].extended + dn;
        double norm = zn.norm().asDouble();

        //
        // Glitch check
        //

        if (norm < ref.xn[iteration].tolerance) {
            break;
        }

        //
        // Period check
        //

        if (opt.periodcheck.enable) {

            if ((dn - p).norm().asDouble() < opt.periodcheck.tolerance) {
                map.set(point, MapEntry {
                    .result     = DR_PERIODIC,
                    .first      = (i32)ref.skipped,
                    .last       = (i32)iteration } );
                return;
            }
            if (iteration == nextUpdate) {
                p = dn;
                nextUpdate *= 1.5;
            }
        }

        //
        // Attractor check
        //

        if (opt.attractorcheck.enable) {

            if (derzn.norm().asDouble() < opt.attractorcheck.tolerance) {
                map.set(point, MapEntry {
                    .result     = DR_ATTRACTED,
                    .first      = (i32)ref.skipped,
                    .last       = (i32)iteration } );
                return;
            }
        }

        //
        // Escape check
        //

        if (norm >= 512) {
        // if (norm >= 4.9) {

            auto nv = zn / ddn;
            nv.normalize();
            map.set(point, MapEntry {
                .result     = DR_ESCAPED,
                .first      = (i32)ref.skipped,
                .last       = (i32)iteration,
                .zn         = StandardComplex(zn),
                .lognorm    = (float)::log(norm),
                .derivative = StandardComplex(ddn),
                .normal     = StandardComplex(nv) } );
            return;
        }
    }

    // If we have drilled up to the maximum depth, the point is (likely) inside
    // the Mandelbrot set. If not, we have to consider this point a glitch
    // point temporarily. Computation has to be repeated with a different
    // reference poin with a larger depth.

    if (limit == opt.location.depth) {

        map.set(point, MapEntry {
            .result     = DR_MAX_DEPTH_REACHED,
            .first      = (i32)ref.skipped,
            .last       = (i32)iteration } );

    } else {

        map.set(point, MapEntry {
            .result     = DR_GLITCH,
            .first      = (i32)ref.skipped,
            .last       = (i32)iteration } );

        glitchPoints.push_back(point);
    }
}

}
