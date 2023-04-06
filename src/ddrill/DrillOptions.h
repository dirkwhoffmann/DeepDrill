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
#include "Palette.h"

namespace dd {

enum class Format { NONE, LOC, MAP, PRF, DIR, TIF };

struct DrillOptions {

    // Set to true to abort the computation
    bool stop = false;

    
    //
    // Key-value pairs (unparsed)
    //
    
    map<string,string> keys;
    
    
    //
    // Key-value pairs (parsed)
    //
        
    string exec;
    string input;
    string output;
    isize verbose;
    bool make;
    
    struct {

        // Center coordinate
        mpf_class real;
        mpf_class imag;

        // Magnification
        mpf_class zoom;
        
        // Maximum iterations count
        isize depth;

    } location;
    
    struct {

        // Image dimensions in pixels
        isize width;
        isize height;
    
        // Fraction of pixels that are allowed to have a wrong color
        double badpixels;

    } image;
    
    struct {
        
        // Color palette
        string values;

    } palette;
    
    struct {
        
        // Tolerance used for glitch detection
        double tolerance;
        
        // Maximum number of rounds
        isize rounds;
                
    } perturbation;
    
    struct {

        // Number of coefficients used in series approximation
        isize coefficients;
        
        // Approximation tolerance
        double tolerance;

    } approximation;
    
    
    //
    // Derived values
    //
    
    // Format of the specified input and output files
    Format inputFormat = Format::NONE;
    Format outputFormat = Format::NONE;

    // The center coordinate
    PrecisionComplex center;

    // Distance between two adjacent pixels
    mpf_class mpfPixelDelta;
    ExtendedDouble pixelDelta;
    
    
    //
    // Initialization
    //

public:

    DrillOptions(map <string,string> &keys);
    void parse();

private:
        
    void derive();
    Format deriveFormat(const string &path);
};

}
