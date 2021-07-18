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
    keys["exec"] = "";
    keys["input"] = "";
    keys["output"] = "";
    keys["verbose"] = "0";
    keys["location.real"] = "0.0";
    keys["location.imag"] = "0.0";
    keys["location.zoom"] = "1.0";
    keys["location.depth"] = "500";
    keys["image.width"] = "960";
    keys["image.height"] = "540";
    keys["palette.values"] = Palette::defaultPalette;
    keys["perturbation.tolerance"] = "1e-12";
    keys["perturbation.maxrounds"] = "50";
    keys["perturbation.accuracy"] = "0.999";
    keys["approximation.coefficients"] = "5";
    keys["approximation.tolerance"] = "1e-12";
    
    // Overwrite defaults with user-defined values
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
        } else if (key == "input") {
            input = value;
        } else if (key == "output") {
            output = value;
        } else if (key == "verbose") {
            verbose = stoi(value);
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
        } else if (key == "palette.values") {
            palette.values = value;
        } else if (key == "perturbation.tolerance") {
            perturbation.tolerance = stod(value);
        } else if (key == "perturbation.maxrounds") {
            perturbation.maxRounds = stod(value);
        } else if (key == "perturbation.accuracy") {
            perturbation.accuracy = stod(value);
        } else if (key == "approximation.coefficients") {
            approximation.coefficients = stoi(value);
        } else if (key == "approximation.tolerance") {
            approximation.tolerance = stod(value);
        } else {
            assert(false);
        }
    }
    deriveVariables();
}

/*
const string *
Options::lookupKey(const string &key)
{
    if (auto it = keys.find(key); it != keys.end()) {
        return &it->second;
    } else {
        throw MissingKeyException("Error: Missing key '" + key + "'");
    }
}

const string *
Options::lookupKey(const string &key, const string &fallback)
{
    try {
        return lookupKey(key);
    } catch (const MissingKeyException &e) {
        keys[key] = fallback;
        return &fallback;
    }
}
*/
 
void
Options::deriveVariables()
{
    // Determine the input and output format
    inputFormat = deriveFormat(input);
    outputFormat = deriveFormat(output);
    
    // Compute the center coordinate
    center = PrecisionComplex(location.real, location.imag);

    // Compute the distance between two pixels on the complex plane
    mpfPixelDelta = mpf_class(4.0) / location.zoom / image.height;
    pixelDelta = mpfPixelDelta;    
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
    
    return Format::NONE;
}

}
