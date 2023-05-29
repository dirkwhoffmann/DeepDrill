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
#include "StandardComplex.h"

namespace dd {

struct MapEntry {

    u32 iteration;
    float lognorm;

    // Experimental
    StandardComplex derivative;
    StandardComplex normal;
};

class DrillMap {

public:
    
    // Configuration options
    const struct Options &opt;

    // Image dimensions
    isize width = 0;
    isize height = 0;
    
    MapEntry *data = nullptr;

public:


    //
    // Initializing
    //

    DrillMap(const Options &options);
    void resize(isize w, isize h);


    //
    // Accessing
    //

    MapEntry *operator [] (const isize &) const;
    MapEntry &get(isize w, isize h) const;
    MapEntry &get(const struct Coord &c) const;
    void set(isize w, isize h, const MapEntry &entry);
    void set(const struct Coord &c, const MapEntry &entry);
    void set(const struct Coord &c, u32 iteration, float lognorm);


    //
    // Loading
    //

public:

    void load(const string &path);
    void load(std::istream &is);

private:

    void loadHeader(std::istream &is);
    void loadChannel(std::istream &is);


    //
    // Saving
    //

public:

    void save(const string &path);
    void save(std::ostream &os);

private:

    void saveHeader(std::ostream &os);
    void saveChannel(std::ostream &os, const string &name);
};

}
