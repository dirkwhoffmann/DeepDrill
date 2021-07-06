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

namespace dd {

struct MissingKeyException : std::runtime_error {
    using std::runtime_error::runtime_error;
};
struct InvalidValueException : std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Options {

public:
    
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
    
    // Rotation angle (not supported yet)
    double rotate;

    // Color palette
    string palette;

    
    //
    // Perturbation parameters
    //

    // Tolerance used for glitch detection
    double glitchTolerance;

    // Maximum number of rounds
    isize maxRounds;
    
    // Fraction of pixels that need to be computed
    double accuracy;
    
    
    //
    // Series approximation parameters
    //

    // Number of coefficients used in series approximation
    isize numCoefficients;
        
    // Debug output level
    isize verbose;
    
    
    //
    // File names
    //
    
    string exportPath;
    string exportName;

    
    //
    // Derived values
    //
    
    // The center coordinate
    PrecisionComplex center;

    // Distance between two adjacent pixels (derived variable)
    mpf_class mpfPixelDelta;
    ExtendedDouble pixelDelta;
    
    
    //
    // Initialization
    //

public:

    Options(map <string,string> &keys) { initialize(keys); }
    void initialize(map <string,string> &keys);

private:
    
    const string *lookupKey(const map <string,string> &keys,
                            const string &key) throws;
    const string *lookupKey(const map <string,string> &keys,
                            const string &key,
                            const string &fallback);

public:
    
    void deriveVariables();
};

}
