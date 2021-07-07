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
#include "Driller.h"

namespace dd {

class Application {
        
public:
        
    // Main entry point
    int main(std::vector <string> &args);
    
private:
        
    // Setup the GMP library
    void setupGmp(std::map <string,string> &keys);
};

}
