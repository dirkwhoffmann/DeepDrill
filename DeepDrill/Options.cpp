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
#include <exception>
#include <filesystem>
#include <iomanip>
#include <iostream>

namespace dd {

void
Options::initialize(map <string,string> &keys)
{
    std::map<string, string>::iterator it;
    string key;

    try {
        
        key = "location.real";
        if (auto value = lookupKey(keys, key)) {
            real = mpf_class(*value);
        }
        key = "location.imag";
        if (auto value = lookupKey(keys, key)) {
            imag = mpf_class(*value);
        }
        key = "location.zoom";
        if (auto value = lookupKey(keys, key)) {
            zoom = mpf_class(*value);
        }
        key = "location.depth";
        if (auto value = lookupKey(keys, key)) {
            depth = stoi(*value);
        }
        key = "image.width";
        if (auto value = lookupKey(keys, key)) {
            width = stoi(*value);
        }
        key = "image.height";
        if (auto value = lookupKey(keys, key)) {
            height = stoi(*value);
        }
        key = "image.rotate";
        if (auto value = lookupKey(keys, key, "0.0")) {
            rotate = stod(*value);
        }
        key = "palette.values";
        if (auto value = lookupKey(keys, key)) {
            palette = *value;
        }
        key = "perturbation.tolerance";
        if (auto value = lookupKey(keys, key, "1e-6")) {
            glitchTolerance = stod(*value);
        }
        key = "perturbation.maxrounds";
        if (auto value = lookupKey(keys, key, "50")) {
            maxRounds = stod(*value);
        }
        key = "approximation.coefficients";
        if (auto value = lookupKey(keys, key, "5")) {
            numCoefficients = stoi(*value);
        }
        key = "debug.verbose";
        if (auto value = lookupKey(keys, key, "0")) {
            verbose = stoi(*value);
        }
        key = "export.path";
        auto current = std::filesystem::current_path().string();
        if (auto value = lookupKey(keys, key, current)) {
            exportPath = *value;
        }
        key = "export.name";
        if (auto value = lookupKey(keys, key)) {
            exportName = *value;
        }

    } catch(const MissingKeyException &e) {
        throw e;
    } catch (const std::exception &e) {
        throw InvalidValueException("Error: Key " + key + " is not valid");
    }
        
    deriveVariables();
    
    if (verbose) {
        
        static int align = 30;
        
        std::cout << std::right << std::setw(align) << "Real: ";
        std::cout << real << std::endl;
        std::cout << std::right << std::setw(align) << "Imag: ";
        std::cout << imag << std::endl;
        std::cout << std::right << std::setw(align) << "Zoom: ";
        std::cout << zoom << std::endl;
        std::cout << std::right << std::setw(align) << "Depth: ";
        std::cout << depth << std::endl;
        std::cout << std::right << std::setw(align) << "Width: ";
        std::cout << width << std::endl;
        std::cout << std::right << std::setw(align) << "Height: ";
        std::cout << height << std::endl;
        std::cout << std::right << std::setw(align) << "Rotate: ";
        std::cout << rotate << std::endl;
        std::cout << std::right << std::setw(align) << "Glitch tolerance: ";
        std::cout << glitchTolerance << std::endl;
        std::cout << std::right << std::setw(align) << "Max rounds: ";
        std::cout << maxRounds << std::endl;
        std::cout << std::right << std::setw(align) << "Coefficients: ";
        std::cout << numCoefficients << std::endl;
        std::cout << std::right << std::setw(align) << "Rotate: ";
        std::cout << rotate << std::endl;
        std::cout << std::right << std::setw(align) << "Export path: ";
        std::cout << exportPath << std::endl;
        std::cout << std::right << std::setw(align) << "Export name: ";
        std::cout << exportName << std::endl << std::endl;

        std::cout << std::right << std::setw(align) << "Center: ";
        std::cout << center << std::endl;
        std::cout << std::right << std::setw(align) << "Pixel delta: ";
        std::cout << mpfPixelDelta << std::endl;
        std::cout << std::right << std::setw(align) << "Precision: ";
        std::cout << mpf_get_default_prec() << std::endl;
    }
}

const string *
Options::lookupKey(const map <string,string> &keys, const string &key)
{
    if (auto it = keys.find(key); it != keys.end()) {
        return &it->second;
    } else {
        throw MissingKeyException("Error: Key " + key + " is missing");
    }
}

const string *
Options::lookupKey(const map <string,string> &keys, const string &key, const string &fallback)
{
    try {
        return lookupKey(keys, key);
    } catch (const MissingKeyException &e) {
        return &fallback;
    }
}

void
Options::deriveVariables()
{
    center = PrecisionComplex(real, imag);
    mpfPixelDelta = mpf_class(4.0) / zoom / height;
    pixelDelta = mpfPixelDelta;
}

}
