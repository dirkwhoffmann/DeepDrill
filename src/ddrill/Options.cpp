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

#include "config.h"
#include "Options.h"
#include "Logger.h"

namespace dd {

Options::Options(map <string,string> &k)
{
    // Register default keys

    // Command line options
    keys["exec"] = "";
    keys["input"] = "";
    keys["output"] = "";
    keys["verbose"] = "0";
    keys["make"] = "0";
    keys["batch"] = "0";

    // Tool keys
    keys["tools.raw2tiff"] = "";
    keys["tools.convert"] = "";

    // Location keys
    keys["location.real"] = "0.0";
    keys["location.imag"] = "0.0";
    keys["location.zoom"] = "1.0";
    keys["location.depth"] = "500";

    // Image keys
    keys["image.width"] = "960";
    keys["image.height"] = "540";
    keys["image.badpixels"] = "0.001";

    // Video keys
    keys["video.framerate"] = "60";
    keys["video.width"] = "480";
    keys["video.height"] = "270";
    keys["video.keyframes"] = "derive";
    keys["video.inbetweens"] = "120";
    keys["video.duration"] = "derive";
    keys["video.bitrate"] = "4096";

    // Palette keys
    keys["palette.values"] = Palette::defaultPalette;

    // Perturbation keys
    keys["perturbation.tolerance"] = "1e-12";
    keys["perturbation.rounds"] = "50";

    // Approximation keys
    keys["approximation.coefficients"] = "5";
    keys["approximation.tolerance"] = "1e-12";

    // Search external tools in some default locations
    std::vector<string> paths = { "/usr/bin", "/usr/local/bin", "/opt/homebrew/bin" };
    for (const auto &path : paths) {
        if (fileExists(path + "/raw2tiff")) keys["tools.raw2tiff"] = path + "/raw2tiff";
        if (fileExists(path + "/convert")) keys["tools.convert"] = path + "/convert";
    }

    // Overwrite default values with the user settings
    for (auto &it : k) {
        if (keys.find(it.first) == keys.end()) {
            throw InvalidValueException("Error: Unknown key '" + it.first + "'");
        }        
        keys[it.first] = it.second;
    }
    
    // Parse all key-value pairs
    parse();
}

void
Options::parse()
{
    for (auto &it : keys) {

        auto &key = it.first;
        auto &value = it.second;
        
        if (key == "exec") {
            exec = value;
        } else if (key == "make") {
            make = stoi(value);
        } else if (key == "batch") {
            batch = stoi(value);
            log::cout.setSilent(batch);
        } else if (key == "input") {
            input = value;
        } else if (key == "output") {
            output = value;
        } else if (key == "verbose") {
            verbose = stoi(value);
        } else if (key == "tools.raw2tiff") {
            tools.raw2tiff = value;
        } else if (key == "tools.convert") {
            tools.convert = value;
        } else if (key == "location.real") {
            location.real = mpf_class(value);
        } else if (key == "location.imag") {
            location.imag = mpf_class(value);
        } else if (key == "location.zoom") {
            location.zoom = mpf_class(value);
        } else if (key == "location.depth") {
            location.depth = stoi(value);
        } else if (key == "image.width") {
            image.width = stoi(value);
        } else if (key == "image.height") {
            image.height = stoi(value);
        } else if (key == "image.badpixels") {
            image.badpixels = stod(value);
        } else if (key == "video.framerate") {
            video.frameRate = stod(value);
        } else if (key == "video.width") {
            video.width = stod(value);
        } else if (key == "video.height") {
            video.height = stod(value);
        } else if (key == "video.keyframes") {
            video.keyframes = value == "derive" ? 0 : stoi(value);
        } else if (key == "video.inbetweens") {
            video.inbetweens = value == "derive" ? 0 : stoi(value);
        } else if (key == "video.duration") {
            video.duration = value == "derive" ? 0 : stoi(value);
        } else if (key == "video.bitrate") {
            video.bitrate = stoi(value);
        } else if (key == "palette.values") {
            palette.values = value;
        } else if (key == "perturbation.tolerance") {
            perturbation.tolerance = stod(value);
        } else if (key == "perturbation.rounds") {
            perturbation.rounds = stod(value);
        } else if (key == "approximation.coefficients") {
            approximation.coefficients = stoi(value);
        } else if (key == "approximation.tolerance") {
            approximation.tolerance = stod(value);
        } else {
            assert(false);
        }
    }
    
    derive();
}
 
void
Options::derive()
{
    // Determine the input and output formats
    inputFormat = deriveFormat(input);
    outputFormat = deriveFormat(output);
    
    // Compute the center coordinate
    center = PrecisionComplex(location.real, location.imag);

    // Compute the distance between two pixels on the complex plane
    mpfPixelDelta = mpf_class(4.0) / location.zoom / image.height;
    pixelDelta = mpfPixelDelta;

    // Derive parameters 'keyframes' and 'inbetweens' if not specified
    if (!video.keyframes) {

        if (video.inbetweens && video.duration) {
            video.keyframes = isize(std::ceil(60.0 * video.duration / video.inbetweens));
        } else {
            video.keyframes = isize(std::ceil(std::log2(location.zoom.get_d())));
        }
    }
    if (!video.inbetweens) {

        if (video.duration) {
            video.inbetweens = isize(std::round(60.0 * video.duration / video.keyframes));
        } else {
            throw Exception("Not enough information to derive key video.inbetweens");
        }
    }

    // Derive or rectify duration parameter
    video.duration = isize(std::round(video.keyframes * video.inbetweens / 60.0));
}

Format
Options::deriveFormat(const string &path)
{
    if (isDirectory(path)) return Format::DIR;

    auto suffix = extractSuffix(path);
    if (suffix == "loc") return Format::LOC;
    if (suffix == "map") return Format::MAP;
    if (suffix == "prf") return Format::PRF;
    if (suffix == "tif" || suffix == "tiff") return Format::TIF;
    if (suffix == "png") return Format::PNG;
    if (suffix == "mpg" || suffix == "mpeg") return Format::MPG;

    return Format::NONE;
}

}
