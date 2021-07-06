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
#include <iostream>
#include <iomanip>

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
 
    std::cout << std::right << std::setw(30) << (description + ": ");
    clock.restart();
}

void
ProgressIndicator::step(isize delta)
{
    progress += delta;

    isize newDots = dotsMax * progress / progressMax;
    for (; dots < newDots; dots++) { std::cout << "."; }
    std::cout.flush();
}

void
ProgressIndicator::done(const string &info)
{
    auto elapsed = clock.stop();
        
    for (; dots < dotsMax; dots++) { std::cout << "."; } std::cout << " ";
    std::cout << elapsed << " sec";
    
    if (info != "") std::cout << " (" << info << ")";
    std::cout << std::endl;
}

}
