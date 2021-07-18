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

#include "config.h"
#include "Driller.h"
#include "Colorizer.h"
#include "Logger.h"
#include "Options.h"
#include "ProgressIndicator.h"

namespace dd {

Driller::Driller(const Options &o, DrillMap &m) : opt(o), map(m)
{

}

void
Driller::drill()
{
    Clock stopWatch;
    vector<Coord> remaining;
    vector<Coord> glitches;
    
    if (opt.verbose) {
        
        log::cout << log::vspace;
        log::cout << log::ralign("Center: ");
        log::cout << opt.center << log::endl;
        log::cout << log::ralign("Magnification: ");
        log::cout << opt.zoom << log::endl;
        log::cout << log::ralign("Pixel delta: ");
        log::cout << opt.mpfPixelDelta << log::endl;
        log::cout << log::ralign("Maximum drill depth: ");
        log::cout << opt.depth << log::endl;
        log::cout << log::ralign("GMP Precision: ");
        log::cout << mpf_get_default_prec() << " Bit" << log::endl;
        log::cout << log::ralign("Perturbation tolerance: ");
        log::cout << opt.perturbationTolerance << log::endl;
        log::cout << log::ralign("Max rounds: ");
        log::cout << opt.maxRounds << log::endl;
        log::cout << log::vspace;
    }
    
    // Allcoate the drill map
    map.resize(opt.width, opt.height);
    
    // Determine the number of tolerated glitched pixels
    isize allowedBadPixels = opt.width * opt.height * (1.0 - opt.accuracy);
        
    // Collect all pixel coordinates to be drilled at
    for (isize y = 0; y < opt.height; y++) {
        for (isize x = 0; x < opt.width; x++) {
            
            remaining.push_back(Coord(x,y));
        }
    }
             
    for (isize round = 1; round <= opt.maxRounds; round++) {

        // Exit if enough pixels have been computed
        if ((isize)remaining.size() <= allowedBadPixels) break;
        
        log::cout << "Round " << round << ": ";
        log::cout << remaining.size() << " points" << log::endl << log::endl;
                
        // Select a reference point
        ref = pickReference(glitches);
        
        // Drill the reference point
        ref.drill(opt);
        
        if (opt.verbose) {
            
            log::cout << log::vspace;
            log::cout << log::ralign("Reference point: ");
            log::cout << ref.coord << log::endl;
            log::cout << log::ralign("Depth: ");
            log::cout << ref.xn.size() << log::endl;
            log::cout << log::vspace;
        }
        
        // If series approximation is enabled...
        if (opt.numCoefficients > 0) {
        
            // Compute the coefficients
            coeff.compute(ref, opt.numCoefficients, opt.depth);

            // Pick the probe points
            pickProbePoints(probePoints);
            
            // Drill the probe points
            ref.skipped = drillProbePoints(probePoints);
            
            if (opt.verbose) {
                
                log::cout << log::vspace;
                log::cout << log::ralign("Skippable iterations: ");
                log::cout << ref.skipped << log::endl;
                log::cout << log::vspace;
            }
        }

        // Drill the remaining pixels
        drill(remaining, glitches);
        remaining = glitches;
        
        if (opt.verbose) {
            
            log::cout << log::vspace;
            log::cout << log::ralign("Glitches: ");
            log::cout << glitches.size() << log::endl;
            log::cout << log::vspace;
        }        
    }
}

ReferencePoint
Driller::pickReference(const vector<Coord> &glitches)
{
    if (glitches.empty()) {

        return ReferencePoint(opt, Coord::center(opt));

    } else {
        
        auto index = rand() % glitches.size();
        return ReferencePoint(opt, glitches[index]);
    }
}

void
Driller::pickProbePoints(vector <Coord> &probes)
{
    static const isize sampling = 2;

    isize width = opt.width - 1;
    isize height = opt.height - 1;
    
    probes.clear();
    
    for (isize i = 0; i < sampling; i++) {
        for (isize j = 0; j < sampling; j++) {
            
            auto x = width * ((double)i / (double)(sampling - 1));
            auto y = height * ((double)j / (double)(sampling - 1));
            
            probes.push_back(Coord(x,y));
        }
    }
}

isize
Driller::drillProbePoints(vector <Coord> &probes)
{
    ProgressIndicator progress("Checking probe points", probes.size());
    
    isize minValid = opt.depth - 1;
    
    for (auto &probe : probes) {

        isize valid = drillProbePoint(probe);
        minValid = std::min(minValid, valid);

        progress.step();
    }
    
    return minValid;
}

isize
Driller::drillProbePoint(Coord &probe)
{
    ExtendedComplex d0 = ref.deltaLocation(opt, probe);
    ExtendedComplex dn = d0;
            
    isize iteration = 0;
    auto tolerance = ExtendedDouble(opt.approximationTolerance);
    
    // The depth of the reference point limits how deep we can drill
    isize limit = ref.xn.size();
    
    // Enter the main loop
    while (++iteration < limit) {
        
        dn *= ref.xn[iteration - 1].extended2 + dn;
        dn += d0;
        dn.reduce();
                
        auto approx = coeff.evaluate(probe, d0, iteration);
        auto error = (approx - dn).norm() / dn.norm();
        error.reduce();
        
        if (error > tolerance) {
            return iteration < 4 ? 0 : iteration - 4;
        }
    }
    return limit;
}

void
Driller::drill(const vector<Coord> &remaining, vector<Coord> &glitches)
{
    ProgressIndicator progress("Computing delta orbits", remaining.size());
    
    glitches.clear();
    for (unsigned int i = 0; i < remaining.size(); i++) {
        
        drill(remaining[i], glitches);
        progress.step(1);
    }
}

void
Driller::drill(const Coord &point, vector<Coord> &glitchPoints)
{
    // Check if this point is the reference point
    /*
    if (point == ref.coord) {
        map.setPixel(ref, palette);
        mapFile.set(ref.coord.x, ref.coord.y, MapEntry { TODO });
        return;
    }
    */
    
    ExtendedComplex d0 = ref.deltaLocation(opt, point);
    ExtendedComplex dn;
    isize iteration = ref.skipped;

    ExtendedComplex approx;
    
    if (ref.skipped) {
        dn = coeff.evaluate(point, d0, ref.skipped);
        approx = dn;
    } else {
        dn = d0;
    }
        
    // The depth of the reference point limits how deep we can drill
    isize limit = ref.xn.size();
            
    // Enter the main loop
    while (++iteration < limit) {
        
        dn *= ref.xn[iteration - 1].extended2 + dn;
        dn += d0;
        dn.reduce();
        
        double norm = (ref.xn[iteration].extended + dn).norm().asDouble();

        // Perform the glitch check
        if (norm < ref.xn[iteration].tolerance) {
            break;
        }
        
        // Perform the escape check
        if (norm >= 256) {
            // map.setPixel(point, palette, iteration, norm);
            map.set(point.x, point.y, MapEntry { (u32)iteration, (float)::log(norm) });
            return;
        }
    }
        
    if (limit == opt.depth) {

        // This point belongs to the Mandelbrot set
        map.set(point.x, point.y, MapEntry { 0, 0 });

    } else {

        // This point is a glitch point
        glitchPoints.push_back(point);
    }
}

}
