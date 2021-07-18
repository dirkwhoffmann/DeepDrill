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

void
Options::initialize()
{
    std::map<string, string>::iterator it;
    string key;

    try {

        key = "input";
        if (auto value = lookupKey(key, "")) {
            input = *value;
        }
        key = "output";
        if (auto value = lookupKey(key, "")) {
            output = *value;
        }
        key = "verbose";
        if (auto value = lookupKey(key, "0")) {
            verbose = stoi(*value);
        }
        key = "location.real";
        if (auto value = lookupKey(key, "0.0")) {
            location.real = mpf_class(*value);
        }
        key = "location.imag";
        if (auto value = lookupKey(key, "0.0")) {
            location.imag = mpf_class(*value);
        }
        key = "location.zoom";
        if (auto value = lookupKey(key, "1")) {
            location.zoom = mpf_class(*value);
        }
        key = "location.depth";
        if (auto value = lookupKey(key, "500")) {
            location.depth = stoi(*value);
        }
        key = "image.width";
        if (auto value = lookupKey(key, "640")) {
            image.width = stoi(*value);
        }
        key = "image.height";
        if (auto value = lookupKey(key, "320")) {
            image.height = stoi(*value);
        }
        key = "palette.values";
        if (auto value = lookupKey(key, "")) {
            palette.palette = *value;
        }
        key = "perturbation.tolerance";
        if (auto value = lookupKey(key, "1e-6")) {
            perturbation.tolerance = stod(*value);
        }
        key = "perturbation.maxrounds";
        if (auto value = lookupKey(key, "50")) {
            perturbation.maxRounds = stod(*value);
        }
        key = "perturbation.accuracy";
        if (auto value = lookupKey(key, "0.999")) {
            perturbation.accuracy = stod(*value);
        }
        key = "approximation.coefficients";
        if (auto value = lookupKey(key, "5")) {
            approximation.coefficients = stoi(*value);
        }
        key = "approximation.tolerance";
        if (auto value = lookupKey(key, "1e-12")) {
            approximation.tolerance = stod(*value);
        }

    } catch(const MissingKeyException &e) {
        throw e;
    } catch (const std::exception &e) {
        throw InvalidValueException("Error: Key " + key + " is not valid");
    }
        
    deriveVariables();
}

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
