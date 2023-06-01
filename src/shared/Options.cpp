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

#include "Options.h"
#include "Exception.h"
#include "IO.h"
#include "Logger.h"

namespace dd {

Options::Options(const AssetManager &assets) : assets(assets)
{
    // Register default keys

    // Location keys
    defaults["location.real"] = "0.0";
    defaults["location.imag"] = "0.0";
    defaults["location.zoom"] = "1.0";
    defaults["location.depth"] = "500";

    // Image keys
    defaults["image.width"] = "960";
    defaults["image.height"] = "540";
    defaults["image.depth"] = "1";
    defaults["image.badpixels"] = "0.001";

    // Video keys
    defaults["video.framerate"] = "60";
    defaults["video.width"] = "0";
    defaults["video.height"] = "0";
    defaults["video.keyframes"] = "0";
    defaults["video.inbetweens"] = "0";
    defaults["video.bitrate"] = "4096";
    defaults["video.scaler"] = "tricubic.glsl";
    defaults["video.illuminator"] = "lambert.glsl";

    // Palette keys
    defaults["palette.mode"] = "default";
    defaults["palette.colors"] = "";
    defaults["palette.scale"] = "1.0";

    // Perturbation keys
    defaults["perturbation.enabled"] = "yes";
    defaults["perturbation.tolerance"] = "1e-6";
    defaults["perturbation.rounds"] = "50";

    // Approximation keys
    defaults["approximation.coefficients"] = "5";
    defaults["approximation.tolerance"] = "1e-12";

    // Debug keys
    defaults["debug.glitches"] = "no";
}

string
Options::version()
{
    return
    std::to_string(VER_MAJOR) + "." + std::to_string(VER_MINOR) +
    (VER_BETA > 0 ? "b" + std::to_string(VER_BETA) : "");
}

void
Options::parse(string key, string value)
{
    keys[key] = value;

    if (key == "location.real") {

        parse(key, value, location.real);

    } else if (key == "location.imag") {

        parse(key, value, location.imag);

    } else if (key == "location.zoom") {

        parse(key, value, location.zoom);

    } else if (key == "location.depth") {

        parse(key, value, location.depth);

    } else if (key == "image.width") {

        parse(key, value, image.width);

        if (image.width == 0) {
            throw KeyValueError(key, "Width must be greater than 0.");
        }
        if (image.width > 3840) {
            throw KeyValueError(key, "Width must be smaller or equal to 3840.");
        }

    } else if (key == "image.height") {

        parse(key, value, image.height);

        if (image.height == 0) {
            throw KeyValueError(key, "Height must be greater than 0.");
        }
        if (image.height > 2160) {
            throw KeyValueError(key, "Height must be less or equal to 2160.");
        }

    } else if (key == "image.depth") {

        parse(key, value, image.depth);

        if (image.depth < 0 || image.depth > 1) {
            throw KeyValueError(key, "Depth must be 0 or 1.");
        }

    } else if (key == "image.badpixels") {

        parse(key, value, image.badpixels);

    } else if (key == "video.framerate") {

        parse(key, value, video.frameRate);

        if (video.frameRate < 25 || video.frameRate > 240) {
            throw KeyValueError(key, "Framerate out of range (25 - 240)");
        }

    } else if (key == "video.width") {

        parse(key, value, video.width);

        if (video.width < 0) {
            throw KeyValueError(key, "Width must not be negative.");
        }
        if (video.width > 1920) {
            throw KeyValueError(key, "Width must be smaller or equal to 1920.");
        }

    } else if (key == "video.height") {

        parse(key, value, video.height);

        if (video.height < 0) {
            throw KeyValueError(key, "Height must not be negative.");
        }
        if (video.height > 1080) {
            throw KeyValueError(key, "Height must be less or equal to 1080.");
        }
        if (video.height % 2 == 1) {
            throw KeyValueError(key, "Height must be dividable by 2.");
        }

    } else if (key == "video.keyframes") {

        parse(key, value, video.keyframes);

    } else if (key == "video.inbetweens") {

        parse(key, value, video.inbetweens);

    } else if (key == "video.bitrate") {

        parse(key, value, video.bitrate);

    } else if (key == "video.scaler") {

        video.scaler = assets.findAsset(value, Format::GLSL);

    } else if (key == "video.illuminator") {

        video.illuminator = assets.findAsset(value, Format::GLSL);

    } else if (key == "palette.mode") {

        parse(key, value, palette.mode);

    } else if (key == "palette.colors") {

        if (value != "") {
            palette.colors = assets.findAsset(value, { Format::BMP, Format::JPG, Format:: PNG });
        }

    } else if (key == "palette.scale") {

        parse(key, value, palette.scale);

    } else if (key == "perturbation.enabled") {

        parse(key, value, perturbation.enabled);

    } else if (key == "perturbation.tolerance") {

        parse(key, value, perturbation.tolerance);

    } else if (key == "perturbation.rounds") {

        parse(key, value, perturbation.rounds);

    } else if (key == "approximation.coefficients") {

        parse(key, value, approximation.coefficients);

    } else if (key == "approximation.tolerance") {

        parse(key, value, approximation.tolerance);

    } else if (key == "debug.glitches") {

        parse(key, value, debug.glitches);

    } else {

        throw KeyValueError(key, "Unexpected key");
    }
}

void
Options::parse(const string &key, const string &value, string &parsed)
{
    parsed = value;
}

void
Options::parse(const string &key, const string &value, bool &parsed)
{
    if (value == "true" || value == "yes" || value == "on") {

        parsed = true;
        return;
    }
    if (value == "false" || value == "no" || value == "off") {

        parsed = false;
        return;
    }

    throw Exception("Invalid argument for key " + key + ": " + value);
}

void
Options::parse(const string &key, const string &value, isize &parsed)
{
    try {
        parsed = stol(value);
    } catch (...) {
        throw Exception("Invalid argument for key " + key + ": " + value);
    }
}

void
Options::parse(const string &key, const string &value, double &parsed)
{
    try {
        parsed = stod(value);
    } catch (...) {
        throw Exception("Invalid argument for key " + key + ": " + value);
    }
}

void
Options::parse(const string &key, const string &value, mpf_class &parsed)
{
    try {
        parsed = mpf_class(value);
    } catch (...) {
        throw Exception("Invalid argument for key " + key + ": " + value);
    }
}

void
Options::parse(const string &key, const string &value, ColoringMode &parsed)
{
    map <string, ColoringMode> modes = {

        { "default", ColoringMode::Default },
        { "relief", ColoringMode::Relief }
    };

    try {
        parsed = modes.at(lowercased(value));
    } catch (...) {
        throw Exception("Invalid argument for key " + key + ": " + value);
    }
}

void
Options::derive()
{
    // Set default values for all missing options
    for (auto &it : defaults) {
        if (keys.find(it.first) == keys.end()) {
            parse(it.first, it.second);
        }
    }

    // Derive unspecified parameters
    auto frameRate = double(video.frameRate);
    auto keyframes = double(video.keyframes);
    auto inbetweens = double(video.inbetweens);

    if (!video.width) {
        video.width = image.width / 2;     // Width can be even or odd
    }
    if (!video.height) {
        video.height = image.height / 2;
        video.height += video.height & 1;  // Height must be even
    }
    if (!video.inbetweens) {
        video.inbetweens = 2 * video.frameRate;
    }
    if (!video.keyframes) {
        video.keyframes = isize(std::ceil(std::log2(location.zoom.get_d())));
    }

    // Derive the video length
    duration = isize(std::round(keyframes * inbetweens / frameRate));

    // Compute the center coordinate
    center = PrecisionComplex(location.real, location.imag);

    // Compute the distance between two pixels on the complex plane
    mpfPixelDelta = mpf_class(4.0) / location.zoom / image.height;
    pixelDelta = mpfPixelDelta;
}

}
