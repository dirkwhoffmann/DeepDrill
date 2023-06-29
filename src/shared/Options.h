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
#include "Colors.h"
#include "Dynamic.h"
#include "IO.h"
#include "PrecisionComplex.h"
#include "ExtendedDouble.h"

namespace dd {

enum class ColoringMode
{
    Default,
    // Textured
};

struct Options {

    // Reference to the asset manager
    const AssetManager &assets;

    // Set to true to interrupt the application
    bool stop = false;


    //
    // Key-value pairs (unparsed)
    //

    // Default keys
    std::map<string,string> defaults;

    // Keys read from configuration files
    std::map<string,string> keys;

    // Keys specified at the command line
    std::vector<string> overrides;

    struct {

        // Indicates if the '-v' flag has been specified
        bool verbose = false;

        // Indicates if the '-b' flag has been specified
        bool batch = false;

    } flags;

    struct {

        // Full path to the executable
        fs::path exec;

        // Full paths to the input and output files
        std::vector<fs::path> inputs;
        std::vector<fs::path> outputs;

    } files;

    struct {

        // Center coordinate
        mpf_class real;
        mpf_class imag;

        // Magnification
        mpf_class zoom;
        
        // Maximum number of iterations
        isize depth;

    } location;

    struct {

        // Drill map dimensions in pixels
        isize width;
        isize height;

        // Indicates if a normal map should be computed
        isize depth;

        // Indicates if map files should be saved in compressed format
        bool compress;
        
    } drillmap;

    struct {

        // Image dimensions in pixels
        isize width;
        isize height;

        // Indicates if an illumination effect should be applied
        isize depth;

        // Path to the illumination filter
        fs::path illuminator;

        // Path to the image downscaling filter
        fs::path scaler;

    } image;

    struct {

        // Video frame rate
        isize frameRate;

        // Number of keyframes
        isize keyframes;

        // Number of in-between images
        isize inbetweens;

        // Experimental (will replace inbetweens)
        Dynamic inbetweens2;

        // Bitrate
        isize bitrate;

        // Path to the video downscaling filter
        fs::path scaler;

    } video;

    struct {

        // Coloring mode
        ColoringMode mode;
        
        // Path to the palette image
        fs::path palette;

        // Path to the texture image
        fs::path texture;

        // Scaling factor
        double scale;

        // Texture opacity
        double opacity;

        // Light vector direction
        double alpha;
        double beta;

    } colors;
    
    struct {

        // Indicates if perturbation should be utilized
        bool enable;

        // Tolerance used for glitch detection
        double tolerance;

        // Fraction of pixels that are allowed to be miscolored
        double badpixels;

        // Maximum number of rounds
        isize rounds;

        // Color used for glitch points
        GpuColor color;

    } perturbation;
    
    struct {

        // Indicates if series approximation should be utilized
        bool enable;

        // Number of coefficients used in series approximation
        isize coefficients;
        
        // Approximation tolerance
        double tolerance;

    } approximation;

    struct {

        // Indicates if area checking should be applied
        bool enable;

        // Color used for points with a positive area check
        GpuColor color;

    } areacheck;

    struct {

        // Indicates if attractor checking should be applied
        bool enable;

        // Tolerance for equality checks
        double tolerance;

        // Color used for points with a positive attractor check
        GpuColor color;

    } attractorcheck;

    struct {

        // Indicates if period checking should be applied
        bool enable;

        // Tolerance for equality checks
        double tolerance;

        // Color unsed for points with a positive period check
        GpuColor color;

    } periodcheck;


    //
    // Initialization
    //

public:

    Options(const AssetManager &assets);


    //
    // Handling input and output files
    //

    std::vector <fs::path> getInputs(Format format);
    std::vector <fs::path> getOutputs(Format format);


    //
    // Parsing key-value pairs
    //

    void parse(string keyvalue);
    void parse(string key, string value);
    void applyDefaults();
    void derive();
};

}
