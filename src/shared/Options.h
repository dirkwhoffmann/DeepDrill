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

enum class Format { NONE, BMP, DIR, JPG, LOC, MAP, MPG, PNG, PRF, PRJ };

inline Format getFormat(const string &path) {

    if (isDirectory(path)) return Format::DIR;

    auto suffix = extractSuffix(path);
    if (suffix == "bmp") return Format::BMP;
    if (suffix == "jpg") return Format::JPG;
    if (suffix == "loc") return Format::LOC;
    if (suffix == "map") return Format::MAP;
    if (suffix == "mpg" || suffix == "mpeg" || suffix == "mov") return Format::MPG;
    if (suffix == "png") return Format::PNG;
    if (suffix == "prf") return Format::PRF;
    if (suffix == "prj") return Format::PRJ;

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

    // Assets search path
    fs::path assets;


    //
    // Key-value pairs (unparsed)
    //

    // Default keys
    map<string,string> defaults;

    // User-defined keys
    map<string,string> keys;
    
    
    //
    // Parsed and derived options
    //

    /*
    string exec; // DEPRECATED
    string input; // DEPRECATED
    string output; // DEPRECATED
    isize verbose; // DEPRECATED
    Format inputFormat = Format::NONE; // DEPRECATED
    Format outputFormat = Format::NONE; // DEPRECATED

    bool make;
    bool batch;
    */

    struct {

        // Indicates if the '-v' flag has been specified
        bool verbose = false;

        // Indicates if the '-m' flag has been specified
        bool make = false;

        // Indicates if the '-b' flag has been specified
        bool batch = false;

    } flags;

    struct {

        // Full path to the executable
        fs::path exec;

        // Full path to the input file
        fs::path input;

        // The input's file type
        Format inputFormat = Format::NONE;

        // Full path to the output file
        fs::path output;

        // The output's file type
        Format outputFormat = Format::NONE;

    } files;

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


    //
    // Parsing key-value pairs
    //

    void parse(string key, string value);
    void derive();

private:

    void parse(const string &key, const string &value, string &parsed);
    void parse(const string &key, const string &value, isize &parsed);
    void parse(const string &key, const string &value, double &parsed);
    void parse(const string &key, const string &value, mpf_class &parsed);


    //
    // Managing assets
    //

public:

    fs::path findAsset(const fs::path &name, const fs::path &dir = "") const;
    fs::path findLocationFile(const fs::path &name) const;
    fs::path findProfile(const fs::path &name) const;
    fs::path findPalette(const fs::path &name) const;
    fs::path findShader(const fs::path &name) const;
};

}
