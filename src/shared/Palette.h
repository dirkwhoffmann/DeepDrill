// -----------------------------------------------------------------------------
// This file is part of DeepDrill
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

namespace dd {

class Palette {
    
public:
    
    // The Default palette
    static const string defaultPalette;

private:
    
    // Configuration options
    const struct Options &opt;
    
    // The color table
    vector <u32> colors;

    
    //
    // Initialization
    //

public:
    
    Palette(const Options &options);
            
    // Initializes the color table
    void init(string values);
    
    // Translates an iteration count into a color
    u32 colorize(isize depth) const;
};

}
