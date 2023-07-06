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

#include "config.h"
#include "Types.h"
#include "Options.h"
#include "Chrono.h"

#include <functional>

namespace dd {

class Parser {

    typedef std::function<void(string,string)> Callback;

public:

    static void ltrim(string &s);
    static void rtrim(string &s);
    static void trim(string &s);
    static void erase(string &s, char c);
    static void tolower(string &s);

    static void parse(const fs::path &path, Callback callback, isize nr = 0);
    static void parse(std::ifstream &stream, Callback callback, isize nr = 0);
    static void parse(std::stringstream &stream, Callback callback, isize nr = 0);

    static void parse(const string &value, string &parsed);
    static void parse(const string &value, fs::path &parsed, const std::vector<Format> &formats);
    static void parse(const string &value, bool &parsed);
    static void parse(const string &value, isize &parsed);
    static void parse(const string &value, isize &parsed, isize min, isize max);
    static void parse(const string &value, double &parsed);
    static void parse(const string &value, double &parsed, double min, double max);
    static void parse(const string &value, mpf_class &parsed);
    static void parse(const string &value, GpuColor &parsed);
    static void parse(const string &value, ColoringMode &parsed);
    static void parse(const string &value, DynamicFloat &parsed);
    static void parse(const string &value, Time &parsed);
    static void parse(const string &value, std::pair<isize,isize> &parsed);
};

}
