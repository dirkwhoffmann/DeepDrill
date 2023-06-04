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

namespace dd {

enum class ColoringMode { Default };

struct Options {

    // Reference to the asset manager
    const AssetManager &assets;

    // Set to true to abort the computation
    bool stop = false;


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

        // Indicates if the '-b' flag has been specified
        bool batch = false;

    } flags;

    struct {

        // Full path to the executable
        fs::path exec;

        // Full path to the input file
        fs::path input;

        // Full path to the output file
        fs::path output;

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

        // Drill map dimensions in pixels
        isize width;
        isize height;

        // Indicates if a normal map should be computed
        isize depth;

    } drillmap;

    struct {

        // Image dimensions in pixels
        isize width;
        isize height;

        // Indicates if a 3D effect should be applied
        isize depth;

        // Path to the downscaling filter
        string scaler;

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
        string illuminator;

    } video;

    struct {

        // Coloring mode
        ColoringMode mode;
        
        // Path to the palette image
        string palette;

        // Scaling factor
        double scale;

        // Light vector direction
        double alpha;
        double beta;

    } colors;
    
    struct {

        // Indicates if perturbation should be used
        bool enabled;

        // Tolerance used for glitch detection
        double tolerance;

        // Fraction of pixels that are allowed to have a wrong color
        double badpixels;

        // Maximum number of rounds
        isize rounds;

    } perturbation;
    
    struct {

        // Number of coefficients used in series approximation
        isize coefficients;
        
        // Approximation tolerance
        double tolerance;

    } approximation;

    struct {

        // Indicates if the color map should be saved
        bool colorMap;

        // Indicates if the normal map should be saved
        bool normalMap;

    } exports;

    struct {

        // Highlight glitches in texture
        bool glitches;

    } debug;


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
    void parse(const string &key, const string &value, bool &parsed);
    void parse(const string &key, const string &value, isize &parsed);
    void parse(const string &key, const string &value, isize &parsed, isize min, isize max);
    void parse(const string &key, const string &value, double &parsed);
    void parse(const string &key, const string &value, mpf_class &parsed);
    void parse(const string &key, const string &value, ColoringMode &parsed);
};

}
