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

/*
struct InvalidValueException : Exception {
    using Exception::Exception;
};

struct UserInterruptException : Exception {
    using Exception::Exception;
};
*/

struct TravelOptions {

    // Set to true to abort the computation
    // bool stop = false;


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

    struct {

        // Center coordinate
        mpf_class real;
        mpf_class imag;

    } location;

    struct {

        // Image dimensions in pixels
        isize width;
        isize height;

    } image;


    //
    // Derived values
    //

    // The center coordinate
    // PrecisionComplex center;


    //
    // Initialization
    //

public:

    TravelOptions(map <string,string> &keys);
    void parse();

private:

    void derive();
};

}
