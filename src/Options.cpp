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
Options::initialize(map <string,string> &keys)
{
    std::map<string, string>::iterator it;
    string key;

    try {

        key = "mapfilein";
        if (auto value = lookupKey(keys, key, "")) {
            mapFileIn = *value;
        }
        key = "mapfileout";
        if (auto value = lookupKey(keys, key, "")) {
            mapFileOut = *value;
        }
        key = "tiff";
        if (auto value = lookupKey(keys, key, "")) {
            tifFileOut = *value;
        }
        key = "verbose";
        if (auto value = lookupKey(keys, key, "0")) {
            verbose = stoi(*value);
        }
        key = "location.real";
        if (auto value = lookupKey(keys, key, "0.0")) {
            real = mpf_class(*value);
        }
        key = "location.imag";
        if (auto value = lookupKey(keys, key, "0.0")) {
            imag = mpf_class(*value);
        }
        key = "location.zoom";
        if (auto value = lookupKey(keys, key, "1")) {
            zoom = mpf_class(*value);
        }
        key = "location.depth";
        if (auto value = lookupKey(keys, key, "500")) {
            depth = stoi(*value);
        }
        key = "image.width";
        if (auto value = lookupKey(keys, key, "640")) {
            width = stoi(*value);
        }
        key = "image.height";
        if (auto value = lookupKey(keys, key, "320")) {
            height = stoi(*value);
        }
        key = "palette.values";
        if (auto value = lookupKey(keys, key, "")) {
            palette = *value;
        }
        key = "perturbation.tolerance";
        if (auto value = lookupKey(keys, key, "1e-6")) {
            perturbationTolerance = stod(*value);
        }
        key = "perturbation.maxrounds";
        if (auto value = lookupKey(keys, key, "50")) {
            maxRounds = stod(*value);
        }
        key = "perturbation.accuracy";
        if (auto value = lookupKey(keys, key, "0.999")) {
            accuracy = stod(*value);
        }
        key = "approximation.coefficients";
        if (auto value = lookupKey(keys, key, "5")) {
            numCoefficients = stoi(*value);
        }
        key = "approximation.tolerance";
        if (auto value = lookupKey(keys, key, "1e-12")) {
            approximationTolerance = stod(*value);
        }

    } catch(const MissingKeyException &e) {
        throw e;
    } catch (const std::exception &e) {
        throw InvalidValueException("Error: Key " + key + " is not valid");
    }
        
    deriveVariables();
    
    if (verbose) {
        
        // log << log::vspace;
        
        static int align = 30;
        
        if (mapFileIn != "") {
            std::cout << std::right << std::setw(align) << "Input map file: ";
            std::cout << mapFileIn << std::endl;
        }
        if (mapFileOut != "") {
            std::cout << std::right << std::setw(align) << "Output map: ";
            std::cout << mapFileOut << std::endl;
        }
        if (tifFileOut != "") {
            std::cout << std::right << std::setw(align) << "Image file: ";
            std::cout << tifFileOut << std::endl;
        }
        std::cout << std::right << std::setw(align) << "Real: ";
        std::cout << real << std::endl;
        std::cout << std::right << std::setw(align) << "Imag: ";
        std::cout << imag << std::endl;
        std::cout << std::right << std::setw(align) << "Zoom: ";
        std::cout << zoom << std::endl;
        std::cout << std::right << std::setw(align) << "Depth: ";
        std::cout << depth << std::endl;
        std::cout << std::right << std::setw(align) << "Image Width: ";
        std::cout << width << std::endl;
        std::cout << std::right << std::setw(align) << "Image Height: ";
        std::cout << height << std::endl;
        std::cout << std::right << std::setw(align) << "Perturbation tolerance: ";
        std::cout << perturbationTolerance << std::endl;
        std::cout << std::right << std::setw(align) << "Max rounds: ";
        std::cout << maxRounds << std::endl;
        std::cout << std::right << std::setw(align) << "Coefficients: ";
        std::cout << numCoefficients << std::endl;

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
        throw MissingKeyException("Error: Missing key '" + key + "'");
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
    // Compute the center coordinate
    center = PrecisionComplex(real, imag);

    // Compute the distance between two pixels on the complex plane
    mpfPixelDelta = mpf_class(4.0) / zoom / height;
    pixelDelta = mpfPixelDelta;    
}

}
