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

#include "commons.h"

namespace dd {

struct MapEntry {

    u32 iteration;
    float lognorm;
};

class DrillMap {
  
public:
    
    isize width = 0;
    isize height = 0;
    double logBailout = 0;

    MapEntry *data = nullptr;

public:
    
    DrillMap() { };
    DrillMap(isize w, isize h);
    DrillMap(const string &path) throws;
    
    void load(const string &path) throws;
    void resize(isize w, isize h);

    const MapEntry &get(isize w, isize h) const;
    void set(isize w, isize h, const MapEntry &entry);
    
    void save(const string &path); 
    void save(std::ostream &stream);
};

}
