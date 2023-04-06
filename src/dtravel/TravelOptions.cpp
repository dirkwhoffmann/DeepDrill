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
#include "TravelOptions.h"
#include "Exception.h"
#include "Logger.h"

namespace dd {

TravelOptions::TravelOptions(map <string,string> &k)
{
    // Register default keys
    keys["exec"] = "";
    keys["input"] = "";
    keys["output"] = "";
    keys["verbose"] = "0";
    keys["location.real"] = "0.0";
    keys["location.imag"] = "0.0";
    keys["image.width"] = "960";
    keys["image.height"] = "540";

    // Overwrite default values with the user settings
    if (!k.empty()) {
        for (auto &it : k) {
            if (keys.find(it.first) == keys.end()) {
                throw InvalidValueException("Error: Unknown key '" + it.first + "'");
            }
            keys[it.first] = it.second;
        }
    }
    
    // Parse all key-value pairs
    parse();
}

void
TravelOptions::parse()
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
        } else if (key == "image.width") {
            image.width = stoi(value);
        } else if (key == "image.height") {
            image.height = stoi(value);
        } else {
            assert(false);
        }
    }

    derive();
}

void
TravelOptions::derive()
{
    // Compute the center coordinate
    // center = PrecisionComplex(location.real, location.imag);
}

}
