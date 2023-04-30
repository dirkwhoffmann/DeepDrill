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
#include "AssetManager.h"
#include "IO.h"
#include "PrecisionComplex.h"
#include "ExtendedDouble.h"

#include <SFML/Graphics.hpp>

namespace dd {

/*
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
*/

struct Options {

    // Reference to the asset manager
    const AssetManager &assets;

    // Set to true to abort the computation
    bool stop = false;

    // Assets search path
    // [[deprecated]] path assets;


    //
    // Key-value pairs (unparsed)
    //

    // Default keys
    map<string,string> defaults;

    // User-defined keys
    map<string,string> keys;

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
        path exec;

        // Full path to the input file
        path input;

        // The input's file type
        // Format inputFormat = Format::NONE;

        // Full path to the output file
        path output;

        // The output's file type
        // Format outputFormat = Format::NONE;

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

        // Path to the palette image
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

    Options(const AssetManager &assets);

    // Returns a version string for this release
    static string version();


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

    /*
public:

    [[deprecated]] path findAsset(const path &name, const path &dir = "") const;
    [[deprecated]] path findLocationFile(const path &name) const;
    [[deprecated]] path findProfile(const path &name) const;
    [[deprecated]] path findPalette(const path &name) const;
    [[deprecated]] path findShader(const path &name) const;
    */
};

}
