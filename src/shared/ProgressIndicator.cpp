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

#include "ProgressIndicator.h"
#include "Logger.h"
#include "IO.h"

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
     
    log::cout << log::ralign(description + ": ");
    log::cout << log::flush;
    clock.restart();
}

void
ProgressIndicator::step(isize delta)
{
    if (clock.isPaused()) return;
    
    progress += delta;

    isize newDots = dotsMax * progress / progressMax;
    for (; dots < newDots; dots++) { log::cout << "."; }
    log::cout << log::flush;
}

void
ProgressIndicator::done(const string &info)
{
    if (clock.isPaused()) return;
    
    auto elapsed = clock.stop();
        
    for (; dots < dotsMax; dots++) { log::cout << "."; } log::cout << " ";
    log::cout << elapsed << " sec";
    
    if (info != "") log::cout << " (" << info << ")";
    log::cout << log::endl;
}

BatchProgressIndicator::BatchProgressIndicator(const string &msg) : msg(msg)
{
    std::stringstream ss;
    Logger logger(ss);

    prefix(logger);
    logger << log::cyan << msg << log::black;
    logger << " ..." << log::endl;

    std::cout << ss.str();
    clock.restart();
}

BatchProgressIndicator::~BatchProgressIndicator()
{
    std::stringstream ss;
    Logger logger(ss);

    prefix(logger);
    logger << log::green << msg;
    logger << log::black << " (" << clock.stop() << " sec)" << log::endl;

    std::cout << ss.str();
}

void
BatchProgressIndicator::prefix(Logger &logger)
{
    auto cnt1 = countFiles("png");
    auto cnt2 = countFiles("loc");
    auto percent = isize(100.0 * cnt1 / cnt2);

    logger << log::blue << "[" << percent << "%] ";
    logger << cnt1 << "/" << cnt2 << ": ";
}

}
