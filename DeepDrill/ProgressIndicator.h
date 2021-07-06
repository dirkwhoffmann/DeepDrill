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

#pragma once

#include "config.h"
#include "Types.h"
#include "Chrono.h"

namespace dd {

class ProgressIndicator {
        
    // Pprogress
    isize progress;
    isize progressMax;
    
    // Printed dots
    isize dots;
    isize dotsMax;
    
    // Stop watch
    Clock clock;
    
    
    //
    // Methods
    //
    
public:
    
    ProgressIndicator(const string &description, isize max = 100);
    ~ProgressIndicator();

    void step(isize delta = 1);

private:
    
    void init(const string &description, isize max = 100);
    void done(const string &info = "");
};

}
