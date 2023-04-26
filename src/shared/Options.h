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
#include "IO.h"
#include "PrecisionComplex.h"
#include "ExtendedDouble.h"

namespace dd {

enum class Format { NONE, LOC, MAP, PRF, DIR, BMP, JPG, PNG, MPG };

inline Format getFormat(const string &path) {

    if (isDirectory(path)) return Format::DIR;

    auto suffix = extractSuffix(path);
    if (suffix == "loc") return Format::LOC;
    if (suffix == "map") return Format::MAP;
    if (suffix == "prf") return Format::PRF;
    if (suffix == "bmp") return Format::BMP;
    if (suffix == "jpg") return Format::JPG;
    if (suffix == "png") return Format::PNG;
    if (suffix == "mpg" || suffix == "mpeg" || suffix == "mov") return Format::MPG;

    return Format::NONE;
}

inline bool isImageFormat(Format format) {
    return
    format == Format::BMP ||
    format == Format::JPG ||
    format == Format::PNG;
}

inline bool isVideoFormat(Format format) {
    return
    format == Format::MPG;
}

struct Options {

    // Set to true to abort the computation
    bool stop = false;

    
    //
    // Key-value pairs (unparsed)
    //

    // Default keys
    map<string,string> defaults;

    // User-defined keys
    map<string,string> keys;
    
    
    //
    // Key-value pairs (parsed)
    //
        
    string exec;
    string input;
    string output;
    isize verbose;
    bool make;
    bool batch;

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

        // Video frame rate
        isize frameRate;

        // Resolution
        isize width;
        isize height;

        // Number of keyframes
        isize keyframes;

        // Number of in-between images
        isize inbetweens;

        // Bitrate
        isize bitrate;

        // Path to fragment shaders
        string scaler;
        string merger;

    } video;

    struct {

        // Path to the color image file
        string colors;

        // Scaling factor
        double scale;
        
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

    // Video duration in seconds
    isize duration;


    //
    // Initialization
    //

public:

    Options();
    void parse(string key, string value);
    void derive();

private:

    void parse(const string &key, const string &value, string &parsed);
    void parse(const string &key, const string &value, isize &parsed);
    void parse(const string &key, const string &value, double &parsed);
    void parse(const string &key, const string &value, mpf_class &parsed);
};

}
