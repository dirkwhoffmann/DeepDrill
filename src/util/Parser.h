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

#pragma once

#include "Types.h"
#include <functional>

namespace dd {

class Parser {
    
public:
    
    static void parse(const string &path, std::function<void(string,string)>callback);
    static void parse(std::ifstream &stream, std::function<void(string,string)>callback);
    static void parse(std::stringstream &stream, std::function<void(string,string)>callback);

private:
    
    static void ltrim(string &s);
    static void rtrim(string &s);
    static void trim(string &s);
    static void tolower(string &s);
};

}
