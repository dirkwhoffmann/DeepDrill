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
#include "ProgressIndicator.h"
#include "Logger.h"

namespace dd {

ProgressIndicator::ProgressIndicator(const string &description, isize max)
{
    init(description, max);
}

ProgressIndicator::~ProgressIndicator()
{
    done();
}

void
ProgressIndicator::init(const string &description, isize max)
{
    progress = 0;
    progressMax = max;
    
    dots = 0;
    dotsMax = 33;
 
    verbose = 1;
    
    log::cout << log::ralign(description + ": ");
    clock.restart();
}

void
ProgressIndicator::step(isize delta)
{
    if (verbose == 0) return;
    
    progress += delta;

    isize newDots = dotsMax * progress / progressMax;
    for (; dots < newDots; dots++) { log::cout << "."; }
    log::cout << log::flush;
}

void
ProgressIndicator::done(const string &info)
{
    if (verbose == 0) return;

    auto elapsed = clock.stop();
        
    for (; dots < dotsMax; dots++) { log::cout << "."; } log::cout << " ";
    log::cout << elapsed << " sec";
    
    if (info != "") log::cout << " (" << info << ")";
    log::cout << log::endl;
    
    verbose = 0;
}

}
