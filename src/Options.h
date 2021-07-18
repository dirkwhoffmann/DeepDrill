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

enum Format { NONE, LOC, MAP, PRF, DIR, TIF };

struct MissingKeyException : std::runtime_error {
    using std::runtime_error::runtime_error;
};
struct InvalidValueException : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct Options {

    //
    // Key-value pairs (orginal)
    //
    
    map<string,string> keys;
    
    
    //
    // Key-value pairs (parsed)
    //
    
    string input;
    string output;
    
    string locFileIn;
    string mapFileIn;
    string mapFileOut;
    string tifFileOut;
    string targetDir;
    
    //
    // Location parameters
    //

    // Complex coordinates of the center
    mpf_class real;
    mpf_class imag;

    // Magnification
    mpf_class zoom;
    
    // Maximum number of iterations
    isize depth;

    
    //
    // Image parameters
    //

    // Image dimensions ins pixels
    isize width;
    isize height;
    
    // Color palette
    string palette;

    
    //
    // Perturbation parameters
    //

    // Tolerance used for glitch detection
    double perturbationTolerance;

    // Maximum number of rounds
    isize maxRounds;
    
    // Fraction of pixels that need to be computed
    double accuracy;
    
    
    //
    // Series approximation parameters
    //

    // Number of coefficients used in series approximation
    isize numCoefficients;
        
    // Tolerance used for glitch detection
    double approximationTolerance;

    
    //
    // Debug parameters
    //
    
    // Debug output level
    isize verbose;
    
    
    //
    // Derived values
    //
    
    // Format of the specified input and output files
    Format inputFormat = NONE;
    Format outputFormat = NONE;

    // The center coordinate
    PrecisionComplex center;

    // Distance between two adjacent pixels (derived variable)
    mpf_class mpfPixelDelta;
    ExtendedDouble pixelDelta;
    
    
    //
    // Initialization
    //

public:

    Options(map <string,string> &k) : keys(k) { initialize(); }
    void initialize();

private:
    
    const string *lookupKey(const string &key) throws;
    const string *lookupKey(const string &key, const string &fallback);

public:
    
    void deriveVariables();
    Format deriveFormat(const string &path);
};

}
