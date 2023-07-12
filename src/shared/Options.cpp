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
#include "AssetManager.h"
#include "Coord.h"
#include "IO.h"
#include "Logger.h"
#include "Parser.h"

namespace dd {

bool Options::stop;

Options::Flags Options::flags;
Options::Files Options::files;
Options::Location Options::location;
Options::Drillmap Options::drillmap;
Options::Mapfile Options::mapfile;
Options::Image Options::image;
Options::Video Options::video;
Options::Palette Options::palette;
Options::Distance Options::distance;
Options::Texture Options::texture;
Options::Lighting Options::lighting;
Options::GPU Options::gpu;
Options::Perturbation Options::perturbation;
Options::Approximation Options::approximation;
Options::Areacheck Options::areacheck;
Options::Attractorcheck Options::attractorcheck;
Options::Periodcheck Options::periodcheck;

std::map<string,string> Options::keys;
std::vector<string> Options::overrides;
std::map<string,string> Options::defaults = [](){

    std::map<string,string> defaults;

    // Location keys
    defaults["location.real"] = "0.0";
    defaults["location.imag"] = "0.0";
    defaults["location.zoom"] = "1.0";
    defaults["location.depth"] = "800";
    defaults["location.escape"] = "1e32";

    // Map keys
    defaults["map.width"] = "1920";
    defaults["map.height"] = "1080";

    // Mapfile keys
    defaults["mapfile.compress"] = "yes";
    defaults["mapfile.result"] = "yes";
    defaults["mapfile.first"] = "yes";
    defaults["mapfile.last"] = "yes";
    defaults["mapfile.nitcnt"] = "yes";
    defaults["mapfile.derivative"] = "no";
    defaults["mapfile.normal"] = "yes";
    defaults["mapfile.dist"] = "yes";

    // Image keys
    defaults["image.width"] = "1920";
    defaults["image.height"] = "1080";

    // Video keys
    defaults["video.framerate"] = "60";
    defaults["video.keyframes"] = "0";
    defaults["video.startframe"] = "0";
    defaults["video.velocity"] = "1.0";
    defaults["video.bitrate"] = "8000";

    // Palette keys
    defaults["palette.image"] = "";
    defaults["palette.bgcolor"] = "black";
    defaults["palette.mode"] = "smooth";
    defaults["palette.scale"] = "1.0";
    defaults["palette.offset"] = "-0.15";

    // Distance estimation keys
    defaults["distance.enable"] = "no";
    defaults["distance.threshold"] = "0.1";

    // Texture keys
    defaults["texture.image"] = "";
    defaults["texture.opacity"] = "0.5";
    defaults["texture.scale"] = "1.0";
    defaults["texture.offset"] = "0.0";

    // Lighting keys
    defaults["lighting.enable"] = "no";
    defaults["lighting.alpha"] = "45";
    defaults["lighting.beta"] = "45";

    // GPU keys
    defaults["gpu.colorizer"] = "gradient.glsl";
    defaults["gpu.illuminator"] = "lambert.glsl";
    defaults["gpu.scaler"] = "tricubic.glsl";

    // Perturbation keys
    defaults["perturbation.enable"] = "yes";
    defaults["perturbation.tolerance"] = "1e-6";
    defaults["perturbation.badpixels"] = "0.001";
    defaults["perturbation.rounds"] = "50";
    defaults["perturbation.color"] = "";

    // Approximation keys
    defaults["approximation.enable"] = "yes";
    defaults["approximation.coefficients"] = "5";
    defaults["approximation.tolerance"] = "1e-12";

    // Area checking keys
    defaults["areacheck.enable"] = "yes";
    defaults["areacheck.color"] = "";

    // Attractor checking keys
    defaults["attractorcheck.enable"] = "yes";
    defaults["attractorcheck.tolerance"] = "1e-34";
    defaults["attractorcheck.color"] = "";

    // Period checking keys
    defaults["periodcheck.enable"] = "yes";
    defaults["periodcheck.tolerance"] = "1e-74";
    defaults["periodcheck.color"] = "";

    return defaults;
}();

std::vector <fs::path>
Options::getInputs(Format format)
{
    std::vector <fs::path> result;

    for (const auto &it : files.inputs) {
        if (AssetManager::getFormat(it) == format) result.push_back(it);
    }
    return result;
}

std::vector <fs::path>
Options::getOutputs(Format format)
{
    std::vector <fs::path> result;

    for (const auto &it : files.outputs) {
        if (AssetManager::getFormat(it) == format) result.push_back(it);
    }
    return result;
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

    parse(key, value);
}

void
Options::parse(string key, string value)
{
    // Convert the key to lower case
    std::transform(key.begin(), key.end(), key.begin(),
                   [](unsigned char ch){ return std::tolower(ch); });

    // Remove white spaces
    erase(value, ' ');

    // Strip quotation marks
    value.erase(remove(value.begin(), value.end(), '\"'), value.end());

    keys[key] = value;

    try {

        if (key == "location.real") {

            Parser::parse(value, location.real);

        } else if (key == "location.imag") {

            Parser::parse(value, location.imag);

        } else if (key == "location.zoom") {

            Parser::parse(value, location.zoom);

        } else if (key == "location.depth") {

            Parser::parse(value, location.depth);

        } else if (key == "location.escape") {

            Parser::parse(value, location.escape);

        } else if (key == "map.width") {

            Parser::parse(value, drillmap.width, MIN_MAP_WIDTH, MAX_MAP_WIDTH);

        } else if (key == "map.height") {

            Parser::parse(value, drillmap.height, MIN_MAP_HEIGHT, MAX_MAP_HEIGHT);

        } else if (key == "mapfile.compress") {

            Parser::parse(value, mapfile.compress);

        } else if (key == "mapfile.result") {

            Parser::parse(value, mapfile.result);

        } else if (key == "mapfile.first") {

            Parser::parse(value, mapfile.first);

        } else if (key == "mapfile.last") {

            Parser::parse(value, mapfile.last);

        } else if (key == "mapfile.nitcnt") {

            Parser::parse(value, mapfile.nitcnt);

        } else if (key == "mapfile.derivative") {

            Parser::parse(value, mapfile.derivative);

        } else if (key == "mapfile.normal") {

            Parser::parse(value, mapfile.normal);

        } else if (key == "mapfile.dist") {

            Parser::parse(value, mapfile.dist);

        } else if (key == "image.width") {

            Parser::parse(value, image.width, MIN_IMAGE_WIDTH, MAX_IMAGE_WIDTH);

        } else if (key == "image.height") {

            Parser::parse(value, image.height, MIN_IMAGE_HEIGHT, MAX_IMAGE_HEIGHT);

            if (image.height % 2 == 1) {
                throw Exception("Height must be dividable by 2");
            }

        } else if (key == "video.framerate") {

            Parser::parse(value, video.frameRate, 25, 240);

        } else if (key == "video.keyframes") {

            Parser::parse(value, video.keyframes);

        } else if (key == "video.startframe") {

            Parser::parse(value, video.startframe);

        } else if (key == "video.velocity") {

            Parser::parse(value, video.velocity);

        } else if (key == "video.bitrate") {

            Parser::parse(value, video.bitrate);

        } else if (key == "palette.image") {

            Parser::parse(value, palette.image, { Format::BMP, Format::JPG, Format:: PNG });

        } else if (key == "palette.bgcolor") {

            Parser::parse(value, palette.bgColor);

        } else if (key == "palette.mode") {

            Parser::parse(value, palette.mode);

        } else if (key == "palette.scale") {

            Parser::parse(value, palette.scale);

        } else if (key == "palette.offset") {

            Parser::parse(value, palette.offset);

        } else if (key == "distance.enable") {

            Parser::parse(value, distance.enable);

        } else if (key == "distance.threshold") {

            Parser::parse(value, distance.threshold);

        } else if (key == "texture.image") {

            Parser::parse(value, texture.image, { Format::BMP, Format::JPG, Format:: PNG });

        } else if (key == "texture.opacity") {

            Parser::parse(value, texture.opacity);

        } else if (key == "texture.scale") {

            Parser::parse(value, texture.scale);

        } else if (key == "texture.offset") {

            Parser::parse(value, texture.offset);

        } else if (key == "lighting.enable") {

            Parser::parse(value, lighting.enable);

        } else if (key == "lighting.alpha") {

            Parser::parse(value, lighting.alpha);

        } else if (key == "lighting.beta") {

            Parser::parse(value, lighting.beta);

        } else if (key == "gpu.colorizer") {

            Parser::parse(value, gpu.colorizer, { Format::GLSL });

        } else if (key == "gpu.illuminator") {

            Parser::parse(value, gpu.illuminator, { Format::GLSL });

        } else if (key == "gpu.scaler") {

            Parser::parse(value, gpu.scaler, { Format::GLSL });

        } else if (key == "areacheck.enable") {

            Parser::parse(value, areacheck.enable);

        } else if (key == "areacheck.color") {

            Parser::parse(value, areacheck.color);

        } else if (key == "attractorcheck.enable") {

            Parser::parse(value, attractorcheck.enable);

        } else if (key == "attractorcheck.tolerance") {

            Parser::parse(value, attractorcheck.tolerance);

        } else if (key == "attractorcheck.color") {

            Parser::parse(value, attractorcheck.color);

        } else if (key == "periodcheck.enable") {

            Parser::parse(value, periodcheck.enable);

        } else if (key == "periodcheck.tolerance") {

            Parser::parse(value, periodcheck.tolerance);

        } else if (key == "periodcheck.color") {

            Parser::parse(value, periodcheck.color);

        } else if (key == "perturbation.enable") {

            Parser::parse(value, perturbation.enable);

        } else if (key == "perturbation.tolerance") {

            Parser::parse(value, perturbation.tolerance);

        } else if (key == "perturbation.badpixels") {

            Parser::parse(value, perturbation.badpixels);

        } else if (key == "perturbation.rounds") {

            Parser::parse(value, perturbation.rounds);

        } else if (key == "perturbation.color") {

            Parser::parse(value, perturbation.color);

        } else if (key == "approximation.enable") {

            Parser::parse(value, approximation.enable);

        } else if (key == "approximation.coefficients") {

            Parser::parse(value, approximation.coefficients);

        } else if (key == "approximation.tolerance") {

            Parser::parse(value, approximation.tolerance);

        } else {

            throw Exception("Unknown key");
        }

    } catch (Exception &e) {
        throw KeyValueError(key, e.what());
    }
}

void
Options::applyDefaults()
{
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

    // Apply overrides
    for (auto &it : overrides) {
        parse(it);
    }
}

void
Options::derive()
{
    // Derive unspecified video parameters
    if (!video.keyframes) {

        auto zoom = ExtendedDouble(location.zoom);
        video.keyframes = isize(std::ceil(zoom.log2().asDouble()));
    }
}

}
