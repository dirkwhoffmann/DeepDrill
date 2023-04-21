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
#include "PrecisionComplex.h"

namespace dd {

struct Coord {

    i16 x;
    i16 y;
    
    Coord() : x(0), y(0) { };
    Coord(i16 nx, i16 ny) : x(nx), y(ny) { };
    Coord(isize nx, isize ny) : x((i16)nx), y((i16)ny) { };
    Coord(double nx, double ny) : x(i16(std::round(nx))), y(i16(std::round(ny))) { };

    bool operator==(const Coord &other) const { return x == other.x && y == other.y; }
    
    static Coord center(const struct Options &opt);
    
    
    //
    // Converting locations
    //
    
    // Translates the coordinate to it's location on the complex plane
    PrecisionComplex translate(const struct Options &opt) const;


    //
    // Computing
    //
    
    inline Coord &operator-=(const Coord &other) {
    
        x -= other.x;
        y -= other.y;
        return *this;
    }
    
    inline Coord operator-(const Coord &other) const {
        
        return Coord { (i16)(x - other.x), (i16)(y - other.y) };
    }

    
    //
    // Pretty printing
    //
    
    friend std::ostream& operator<<(std::ostream& os, const Coord& value);
};

}
