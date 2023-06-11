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

    // Keys specified at the command line
    std::vector <string> overrides;

    // Default keys
    std::map <string,string> defaults;

    // User-defined keys
    std::map <string,string> keys;

    struct {

        // Indicates if the '-v' flag has been specified
        bool verbose = false;

        // Indicates if the '-b' flag has been specified
        bool batch = false;

    } flags;

    struct {

        // Full path to the executable
        fs::path exec;

        // Full paths to input files, output files, and profiles
        std::vector <fs::path> inputs;
        std::vector <fs::path> outputs;
        std::vector <fs::path> profiles;

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

        // Path to the illumination filter
        string illuminator;

        // Path to the downscaling filter
        string scaler;

    } image;

    struct {

        // Video frame rate
        isize frameRate;

        // Number of keyframes
        isize keyframes;

        // Number of in-between images
        isize inbetweens;

        // Bitrate
        isize bitrate;

        // Path to fragment shaders
        string scaler;

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
        bool enable;

        // Tolerance used for glitch detection
        double tolerance;

        // Fraction of pixels that are allowed to have a wrong color
        double badpixels;

        // Maximum number of rounds
        isize rounds;

    } perturbation;
    
    struct {

        // Indicates if series approximation should be used
        bool enable;

        // Number of coefficients used in series approximation
        isize coefficients;
        
        // Approximation tolerance
        double tolerance;

    } approximation;

    struct {

        // Highlight glitches in texture
        bool glitches;

    } debug;


    //
    // Derived values
    //

    // Center coordinate in precision format
    PrecisionComplex center;

    // Bounding box in precision format
    mpf_class x0, y0, x1, y1;

    // Distance between two adjacent pixels
    mpf_class mpfPixelDeltaX;
    mpf_class mpfPixelDeltaY;
    ExtendedDouble pixelDeltaX;
    ExtendedDouble pixelDeltaY;

    // Video duration in seconds
    isize duration;


    //
    // Initialization
    //

public:

    Options(const AssetManager &assets);


    //
    // Parsing key-value pairs
    //

    void parse(string keyvalue);
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
