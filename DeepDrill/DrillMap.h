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

#include "Types.h"
#include "Coord.h"
#include "Options.h"

namespace dd {

class DrillMap {

public:
    
    // Configuration options
    const Options &opt;
    
    // Image data
    u32 *image = nullptr;

    
    //
    // Initializing
    //

public:
    
    DrillMap(const Options &options);
    ~DrillMap();

    
    //
    // Colorizing
    //
    
public:
    
    void setPixel(Coord c, u32 color) { image[c.y * opt.width + c.x] = color; }
    void setPixel(isize x, isize y, u32 color) { image[y * opt.width + x] = color; }

        
    //
    // Exporting
    //
    
public:
    
    void saveImage();
    void saveGlitchImage(const vector<Coord> &glitches, isize round);
    void save(const string &name, u32 *image);
};

}
