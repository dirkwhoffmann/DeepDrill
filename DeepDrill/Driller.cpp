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
#include "ProgressIndicator.h"
#include <iostream>
#include <iomanip>

namespace dd {

Driller::Driller(const Options &options) : opt(options)
{
    palette.init(opt.palette);
}

void Driller::drill() {

    vector<Coord> remaining;
    vector<Coord> glitches;

    Clock stopWatch;
    
    // Determine the number of tolerated glitched pixels
    isize allowedBadPixels = opt.width * opt.height * badPixels;
    
    // Collect all pixel coordinates to be drilled at
    for (isize y = 0; y < opt.height; y++) {
        for (isize x = 0; x < opt.width; x++) {
            
            remaining.push_back(Coord(x,y));
        }
    }
             
    for (isize round = 1; round <= opt.maxRounds; round++) {

        // Exit if enough pixels have been computed
        if (remaining.size() <= allowedBadPixels) break;
        
        std::cout << std::endl << "Round " << round << ": ";
        std::cout << remaining.size() << " points" << std::endl << std::endl;
                
        // Select a reference point
        ref = pickReference(glitches);
        
        // Drill the reference point
        ref.drill(opt);
        
        if (opt.verbose) {
            
            std::cout << std::endl;
            std::cout << RALIGN << "Reference point: ";
            std::cout << ref.coord << std::endl;
            std::cout << RALIGN << "Depth: ";
            std::cout << ref.xn.size() << std::endl;
            std::cout << std::endl;
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
                
                std::cout << std::endl;
                std::cout << RALIGN << "Skippable iterations: ";
                std::cout << ref.skipped << std::endl;
                std::cout << std::endl;
            }
        }

        // Drill the remaining pixels
        drill(remaining, glitches);
        remaining = glitches;
    }
    
    auto elapsed = stopWatch.stop();
    map.saveImage();

    std::cout << std::endl << "Total time: " << elapsed.asString() << std::endl;
}

ReferencePoint
Driller::pickReference(const vector<Coord> &glitches)
{
    if (glitches.empty()) {
                                         
        return ReferencePoint(opt, Coord::center(opt));

    } else {
        
        auto index = rand() & glitches.size();
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
    
    // The depth of the reference point limits how deep we can drill
    isize limit = ref.xn.size();
    
    // Enter the main loop
    while (++iteration < limit) {
        
        dn *= ref.xn[iteration - 1].extended2 + dn;
        dn += d0;
        dn.reduce();
                
        auto approx = coeff.evaluate(probe, d0, iteration);
        auto error = (approx - dn).norm() / dn.norm();

        if (error > 1e-12) {
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
        if (norm < ref.xn[iteration].tolerance) break;
        
        // Perform the escape check
        if (norm >= 256) {

            int index = (int)((iteration - log2(log2(norm))) * 5);
            map.setPixel(point, palette.colorize(index));
            return;
        }
    }
    
    if (limit == opt.depth) {

        // This point belongs to the Mandelbrot set
        map.setPixel(point, 0);
        
    } else {

        // This point is a glitch point
        glitchPoints.push_back(point);
    }
}

}
