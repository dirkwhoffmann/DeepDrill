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

#include "MathTypes.h"
#include "Options.h"
#include "Palette.h"
#include "MapFile.h"

namespace dd {

class Colorizer {

    // Configuration options
    const Options &opt;
    
    // The associated map file
    const MapFile &map;
    
    // The image data
    u32 *image = nullptr;

    // The color palette
    Palette palette = Palette(opt);

    
    //
    // Initialization
    //

public:

    Colorizer(const Options &options, const MapFile &map);
    ~Colorizer();
    
    
    //
    // Colorizing the Mandelbrot set
    //

    void colorize();
    void colorize(class Coord c);

    
    //
    // Exporting
    //
    
    void save(const string &path) throws;
};

}