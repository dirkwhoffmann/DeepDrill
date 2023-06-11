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
#include "Coord.h"
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
    defaults["location.depth"] = "800";

    // Map keys
    defaults["map.width"] = "1920";
    defaults["map.height"] = "1080";
    defaults["map.depth"] = "1";

    // Image keys
    defaults["image.width"] = "1920";
    defaults["image.height"] = "1080";
    defaults["image.depth"] = "0";
    defaults["image.illuminator"] = "lambert.glsl";
    defaults["image.scaler"] = "bicubic.glsl";

    // Video keys
    defaults["video.framerate"] = "60";
    defaults["video.keyframes"] = "0";
    defaults["video.inbetweens"] = "0";
    defaults["video.bitrate"] = "8000";
    defaults["video.scaler"] = "tricubic.glsl";

    // Palette keys
    defaults["colors.mode"] = "default";
    defaults["colors.palette"] = "";
    defaults["colors.scale"] = "1.0";
    defaults["colors.alpha"] = "45";
    defaults["colors.beta"] = "45";

    // Perturbation keys
    defaults["perturbation.enable"] = "yes";
    defaults["perturbation.tolerance"] = "1e-6";
    defaults["perturbation.badpixels"] = "0.001";
    defaults["perturbation.rounds"] = "50";

    // Approximation keys
    defaults["approximation.enable"] = "yes";
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
Options::parse(string keyvalue)
{
    // Seek the '=' symbol
    auto pos = keyvalue.find("=");
    if (pos == std::string::npos) throw SyntaxError("Parse error");

    // Split string
    auto key = keyvalue.substr(0, pos);
    auto value = keyvalue.substr(pos + 1, std::string::npos);

    // Convert the key to lower case
    std::transform(key.begin(), key.end(), key.begin(),
                   [](unsigned char ch){ return std::tolower(ch); });

    parse(key, value);
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

    } else if (key == "map.width") {

        parse(key, value, drillmap.width, MIN_MAP_WIDTH, MAX_MAP_WIDTH);

    } else if (key == "map.height") {

        parse(key, value, drillmap.height, MIN_MAP_HEIGHT, MAX_MAP_HEIGHT);

    } else if (key == "map.depth") {

        parse(key, value, drillmap.depth, 0, 1);

    } else if (key == "image.width") {

        parse(key, value, image.width, MIN_IMAGE_WIDTH, MAX_IMAGE_WIDTH);

    } else if (key == "image.height") {

        parse(key, value, image.height, MIN_IMAGE_HEIGHT, MAX_IMAGE_HEIGHT);

        if (image.height % 2 == 1) {
            throw KeyValueError(key, "Height must be dividable by 2.");
        }

    } else if (key == "image.depth") {

        parse(key, value, image.depth, 0, 1);

    } else if (key == "image.illuminator") {

        image.illuminator = assets.findAsset(value, Format::GLSL);

    } else if (key == "image.scaler") {

        image.scaler = assets.findAsset(value, Format::GLSL);

    } else if (key == "video.framerate") {

        parse(key, value, video.frameRate, 25, 240);

    } else if (key == "video.keyframes") {

        parse(key, value, video.keyframes);

    } else if (key == "video.inbetweens") {

        parse(key, value, video.inbetweens);

    } else if (key == "video.bitrate") {

        parse(key, value, video.bitrate);

    } else if (key == "video.scaler") {

        video.scaler = assets.findAsset(value, Format::GLSL);

    } else if (key == "colors.mode") {

        parse(key, value, colors.mode);

    } else if (key == "colors.palette") {

        if (value != "") {
            colors.palette = assets.findAsset(value, { Format::BMP, Format::JPG, Format:: PNG });
        }

    } else if (key == "colors.scale") {

        parse(key, value, colors.scale);

    } else if (key == "colors.alpha") {

        parse(key, value, colors.alpha);

        if (colors.alpha < 0.0 || colors.alpha >= 360.0) {
            throw KeyValueError(key, "Angle out of range");
        }

    } else if (key == "colors.beta") {

        parse(key, value, colors.beta);

        if (colors.beta < 0.0 || colors.beta >= 360.0) {
            throw KeyValueError(key, "Angle out of range");
        }

    } else if (key == "perturbation.enable") {

        parse(key, value, perturbation.enable);

    } else if (key == "perturbation.tolerance") {

        parse(key, value, perturbation.tolerance);

    } else if (key == "perturbation.badpixels") {

        parse(key, value, perturbation.badpixels);

    } else if (key == "perturbation.rounds") {

        parse(key, value, perturbation.rounds);

    } else if (key == "approximation.enable") {

        parse(key, value, approximation.enable);

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
Options::parse(const string &key, const string &value, isize &parsed, isize min, isize max)
{
    parse(key, value, parsed);

    if (parsed < min) {
        throw Exception("Invalid argument for key " + key +
                        ": Value must be >= " + std::to_string(min));
    }
    if (parsed > max) {
        throw Exception("Invalid argument for key " + key +
                        ": Value must be <= " + std::to_string(max));
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
    std::map <string, ColoringMode> modes = {

        { "default", ColoringMode::Default },
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
    // Apply overrides
    for (auto &it : overrides) parse(it);

    // Adjust some default values
    if (keys.find("image.width") != keys.end()) {
        defaults["map.width"] = keys["image.width"];
    }
    if (keys.find("image.height") != keys.end()) {
        defaults["map.height"] = keys["image.height"];
    }

    // Use default values for all missing keys
    for (auto &it : defaults) {
        if (keys.find(it.first) == keys.end()) {
            parse(it.first, it.second);
        }
    }

    // Derive unspecified video parameters
    auto frameRate = double(video.frameRate);
    auto keyframes = double(video.keyframes);
    auto inbetweens = double(video.inbetweens);

    if (!video.inbetweens) {
        video.inbetweens = 2 * video.frameRate;
    }
    if (!video.keyframes) {
        video.keyframes = isize(std::ceil(std::log2(location.zoom.get_d())));
    }

    // Derive the video length
    duration = isize(std::round(keyframes * inbetweens / frameRate));

    // Compute the distance between two pixels on the complex plane
    mpfPixelDeltaX = mpfPixelDeltaY = mpf_class(4.0) / location.zoom / drillmap.height;
    pixelDeltaX = pixelDeltaY = mpfPixelDeltaY;

    // Derive coordinates
    center = PrecisionComplex(location.real, location.imag);
    auto x0y0 = Coord::ul(*this).translate(*this);
    auto x1y1 = Coord::lr(*this).translate(*this);
    x0 = x0y0.re;
    y0 = x0y0.im;
    x1 = x1y1.re;
    y1 = x1y1.im;
}

}
