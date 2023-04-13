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

Options::Options()
{
    // Register default keys

    // Tool keys
    defaults["tools.raw2tiff"] = "";
    defaults["tools.convert"] = "";

    // Location keys
    defaults["location.real"] = "0.0";
    defaults["location.imag"] = "0.0";
    defaults["location.dreal"] = "0.0";
    defaults["location.dimag"] = "0.0";
    defaults["location.zoom"] = "1.0";
    defaults["location.depth"] = "500";

    // Image keys
    defaults["image.width"] = "960";
    defaults["image.height"] = "540";
    defaults["image.badpixels"] = "0.001";

    // Animation keys
    defaults["animation.dx"] = "0";
    defaults["animation.dy"] = "0";

    // Video keys
    defaults["video.framerate"] = "60";
    defaults["video.width"] = "480";
    defaults["video.height"] = "270";
    defaults["video.keyframes"] = "derive";
    defaults["video.inbetweens"] = "120";
    defaults["video.duration"] = "derive";
    defaults["video.bitrate"] = "4096";
    defaults["video.scaler"] = "";

    // Palette keys
    defaults["palette.values"] = Palette::defaultPalette;

    // Perturbation keys
    defaults["perturbation.tolerance"] = "1e-12";
    defaults["perturbation.rounds"] = "50";

    // Approximation keys
    defaults["approximation.coefficients"] = "5";
    defaults["approximation.tolerance"] = "1e-12";

    // Search for external tools...
    std::vector<string> paths = { "/usr/bin", "/usr/local/bin", "/opt/homebrew/bin" };
    for (const auto &path : paths) {
        if (fileExists(path + "/raw2tiff")) defaults["tools.raw2tiff"] = path + "/raw2tiff";
        if (fileExists(path + "/convert")) defaults["tools.convert"] = path + "/convert";
    }
}

void
Options::parse(map <string,string> &keymap)
{
    // Add default keys
    if (keys.empty()) {
        for (const auto &it : defaults) {
            keys[it.first] = it.second;
        }
    }

    // Overwrite default values with the user settings
    for (auto &it : keymap) {
        if (defaults.find(it.first) == keys.end()) {
            throw Exception("Unknown key '" + it.first + "'");
        }
        keys[it.first] = it.second;
    }

    for (auto &it : keys) {
        parse(it.first, it.second);
    }

    check();
    derive();
}

void
Options::parse(string key, string value)
{
    keys[key] = value;

    if (key == "tools.raw2tiff") {
        tools.raw2tiff = value;
    } else if (key == "tools.convert") {
        tools.convert = value;
    } else if (key == "location.real") {
        location.real = mpf_class(value);
    } else if (key == "location.imag") {
        location.imag = mpf_class(value);
    } else if (key == "location.dreal") {
        location.dreal = mpf_class(value);
    } else if (key == "location.dimag") {
        location.dimag = mpf_class(value);
    } else if (key == "location.zoom") {
        location.zoom = mpf_class(value);
    } else if (key == "location.depth") {
        location.depth = stol(value);
    } else if (key == "image.width") {
        image.width = stoi(value);
    } else if (key == "image.height") {
        image.height = stoi(value);
    } else if (key == "image.badpixels") {
        image.badpixels = stod(value);
    } else if (key == "animation.dx") {
        animation.dx = stoi(value);
    } else if (key == "animation.dy") {
        animation.dy = stoi(value);
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
    } else if (key == "video.scaler") {
        video.scaler = value;
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

/*
void
Options::parseDefaults()
{
    for (auto &it : defaults) {
        if (keys.find(it.first) == keys.end()) {
            printf("Adding default for key %s\n", it.first.c_str());
            parse(it.first, it.second);
        }
    }
}
*/

void
Options::check()
{
    if (video.frameRate < 25 || video.frameRate > 240) {
        throw InvalidValueException("video.framrate", video.frameRate);
    }
}

void
Options::derive()
{
    auto format = [](const string &path) {

        if (isDirectory(path)) return Format::DIR;

        auto suffix = extractSuffix(path);
        if (suffix == "loc") return Format::LOC;
        if (suffix == "map") return Format::MAP;
        if (suffix == "prf") return Format::PRF;
        if (suffix == "tif" || suffix == "tiff") return Format::TIF;
        if (suffix == "png") return Format::PNG;
        if (suffix == "mpg" || suffix == "mpeg") return Format::MPG;

        return Format::NONE;
    };

    // Set default values for all missing options
    for (auto &it : defaults) {
        if (keys.find(it.first) == keys.end()) {
            printf("Adding default for key %s\n", it.first.c_str());
            parse(it.first, it.second);
        }
    }

    // Determine the input and output formats
    inputFormat = format(input);
    outputFormat = format(output);

    // Compute the center coordinate
    center = PrecisionComplex(location.real + location.dreal, location.imag + location.dimag);

    // Compute the distance between two pixels on the complex plane
    mpfPixelDelta = mpf_class(4.0) / location.zoom / image.height;
    pixelDelta = mpfPixelDelta;

    // Derive unspecified parameters
    auto frameRate = double(video.frameRate);
    auto keyframes = double(video.keyframes);
    auto duration = double(video.duration);
    auto inbetweens = double(video.inbetweens);

    if (!video.keyframes) {

        if (video.inbetweens && video.duration) {
            video.keyframes = isize(std::ceil(frameRate * duration / inbetweens));
        } else {
            video.keyframes = isize(std::ceil(std::log2(location.zoom.get_d())));
        }
    }
    if (!video.inbetweens) {

        if (video.duration) {
            video.inbetweens = isize(std::round(frameRate * duration / keyframes));
        } else {
            throw Exception("Not enough information to derive key video.inbetweens");
        }
    }

    // Derive or rectify duration parameter
    video.duration = isize(std::round(keyframes * inbetweens / frameRate));
}

}
