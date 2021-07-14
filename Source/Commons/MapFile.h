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
#include "Exception.h"
#include <fstream>

namespace dd {

struct MapEntry {

    u32 iteration;
    float lognorm;
};

class MapFile {
  
    isize width = 0;
    isize height = 0;
    double logBailout = 0;

    MapEntry *data = nullptr;

public:
    
    MapFile() { };
    MapFile(isize w, isize h);
    MapFile(const string &path) throws;
    
    void resize(isize w, isize h);
    
    const MapEntry &get(isize w, isize h);
    void set(isize w, isize h, const MapEntry &entry);
    
    void save(const string &path); 
    void save(std::ostream &stream);
};

}
