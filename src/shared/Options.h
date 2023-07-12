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
#include "Exception.h"
#include "Colors.h"
#include "DynamicFloat.h"
#include "ExtendedDouble.h"
#include "PrecisionComplex.h"

namespace dd {

enum class ColoringMode
{
    Classic,
    Smooth
};

struct Options {

    // Set to true to interrupt the application
    static bool stop;


    //
    // Key-value pairs (unparsed)
    //

    // Default keys
    static std::map<string,string> defaults;

    // Keys read from configuration files
    static std::map<string,string> keys;

    // Keys specified at the command line
    static std::vector<string> overrides;

    static struct Flags {

        // Indicates if the '-v' flag has been specified
        bool verbose = false;

        // Indicates if the '-b' flag has been specified
        bool batch = false;

    } flags;

    static struct Files {

        // Full path to the executable
        fs::path exec;

        // Full paths to the input and output files
        std::vector<fs::path> inputs;
        std::vector<fs::path> outputs;

    } files;

    static struct Location {

        // Center coordinate
        mpf_class real;
        mpf_class imag;

        // Magnification
        mpf_class zoom;
        
        // Maximum number of iterations
        isize depth;

        // Escape radius
        double escape;

    } location;

    static struct Drillmap {

        // Drill map dimensions in pixels
        isize width;
        isize height;
        
    } drillmap;

    static struct Mapfile {

        // Indicates if map files should be saved in compressed format
        bool compress;

        // Channels which are saved in the mapfile
        bool result;
        bool first;
        bool last;
        bool nitcnt;
        bool derivative;
        bool normal;
        bool dist;

    } mapfile;

    static struct Image {

        // Image dimensions in pixels
        isize width;
        isize height;

    } image;

    static struct Video {

        // Video frame rate
        isize frameRate;

        // Number of keyframes
        isize keyframes;

        // The first visible keyframe
        isize startframe;

        // Zoom velocity
        DynamicFloat velocity;

        // Bitrate
        isize bitrate;

    } video;

    static struct Palette {

        // Path to the palette image
        fs::path image;

        // Background color
        GpuColor bgColor;
        
        // Coloring mode
        ColoringMode mode;

        // Scaling and shifting
        DynamicFloat scale;
        DynamicFloat offset;

    } palette;

    static struct Distance {

        // Indicates if distance estimation should be applied
        bool enable;

        // Thickness
        DynamicFloat threshold;

    } distance;

    static struct Texture {

        // Path to the texture image
        fs::path image;

        // Texture opacity
        DynamicFloat opacity;

        // Scaling and shifting
        DynamicFloat scale;
        DynamicFloat offset;

    } texture;

    static struct Lighting {

        // Indicates if a spatial images shall be computed
        bool enable;

        // Direction of the light vector
        DynamicFloat alpha;
        DynamicFloat beta;

    } lighting;

    static struct GPU {

        // Path to the colorization filter
        fs::path colorizer;

        // Path to the illumination filter
        fs::path illuminator;

        // Path to the downscaling filter
        fs::path scaler;

    } gpu;

    static struct Perturbation {

        // Indicates if perturbation shall be utilized
        bool enable;

        // Tolerance used for glitch detection
        double tolerance;

        // Fraction of pixels that are allowed to be miscolored
        double badpixels;

        // Maximum number of rounds
        isize rounds;

        // Optional debug color for glitch points
        std::optional<GpuColor> color;

    } perturbation;
    
    static struct Approximation {

        // Indicates if series approximation shall be utilized
        bool enable;

        // Number of coefficients used in series approximation
        isize coefficients;
        
        // Approximation tolerance
        double tolerance;

    } approximation;

    static struct Areacheck {

        // Indicates if area checking should be applied
        bool enable;

        // Optional debug color for points with a positive area check
        std::optional<GpuColor> color;

    } areacheck;

    static struct Attractorcheck {

        // Indicates if attractor checking should be applied
        bool enable;

        // Tolerance for equality checks
        double tolerance;

        // Color used for points with a positive attractor check
        std::optional<GpuColor> color;

    } attractorcheck;

    static struct Periodcheck {

        // Indicates if period checking should be applied
        bool enable;

        // Tolerance for equality checks
        double tolerance;

        // Color unsed for points with a positive period check
        std::optional<GpuColor> color;

    } periodcheck;


    //
    // Initialization
    //

public:

    // This class is not meant to be instantiated
    Options() = delete;


    //
    // Handling input and output files
    //

    static std::vector <fs::path> getInputs(Format format);
    static std::vector <fs::path> getOutputs(Format format);


    //
    // Parsing key-value pairs
    //

    static void parse(string keyvalue);
    static void parse(string key, string value);
    static void applyDefaults();
    static void derive();
};

}
