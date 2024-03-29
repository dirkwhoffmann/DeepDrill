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

#include "Parser.h"
#include "Exception.h"
#include "AssetManager.h"
#include "IO.h"
#include "Logger.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace dd {

void
Parser::parse(const fs::path &path, Callback callback, isize nr)
{
    auto fs = std::ifstream(path);

    if (!fs.is_open()) {
        throw Exception("Failed to open file " + path.string() + ".");
    }
 
    try { parse(fs, callback, nr); } catch (ParseError &e) {

        e.path = path.filename();
        throw;
    }
}

void
Parser::parse(std::ifstream &stream, Callback callback, isize nr)
{
    std::stringstream ss;
    ss << stream.rdbuf();
    
    parse(ss, callback, nr);
}

void
Parser::parse(std::stringstream &stream, Callback callback, isize nr)
{
    isize line = 0;
    string input;
    string section;

    try {

        while(std::getline(stream, input)) {

            line++;

            // Remove comments
            input = input.substr(0, input.find("#"));

            // Remove white spaces
            erase(input, ' ');

            // Ignore empty lines
            if (input == "") continue;

            // Check if this line contains a section marker
            if (input.front() == '[' && input.back() == ']') {

                section = input.substr(1, input.size() - 2);
                tolower(section);
                continue;
            }

            // Check if this line is a key-value pair
            if (auto pos = input.find("="); pos != std::string::npos) {

                auto key = input.substr(0, pos);
                auto value = input.substr(pos + 1, std::string::npos);

                // Check if the key is prefixed with a frame range
                if (auto pos2 = key.find(":"); pos2 != std::string::npos) {

                    auto prefix = key.substr(0, pos2);
                    key = key.substr(pos2 + 1, std::string::npos);

                    std::pair<isize,isize> range = { 0, LONG_MAX };
                    parse(prefix, range);

                    // Only proceed if the frame is inside the valid range
                    if (nr < range.first || nr > range.second) continue;
                }

                // Process the key-value pair
                callback(section + "." + key ,value);
            }
        }

    } catch (Exception &e) {

        throw ParseError(e, line);
    }
}

void
Parser::parse(const string &value, string &parsed)
{
    parsed = value;
}

void
Parser::parse(const string &value, fs::path &parsed, const std::vector<Format> &formats)
{
    parsed = value == "" ? "" : AssetManager::findAsset(value, formats);
}

void
Parser::parse(const string &value, bool &parsed)
{
    if (value == "true" || value == "yes" || value == "on") {

        parsed = true;
        return;
    }
    if (value == "false" || value == "no" || value == "off") {

        parsed = false;
        return;
    }

    throw Exception("Invalid argument: " + value);
}

void
Parser::parse(const string &value, isize &parsed)
{
    try {
        parsed = stol(value);
    } catch (...) {
        throw Exception("Invalid argument: " + value);
    }
}

void
Parser::parse(const string &value, isize &parsed, isize min, isize max)
{
    parse(value, parsed);

    if (parsed < min) {
        throw Exception("Invalid argument. Value must be >= " + std::to_string(min));
    }
    if (parsed > max) {
        throw Exception("Invalid argument. Value must be <= " + std::to_string(max));
    }
}

void
Parser::parse(const string &value, u32 &parsed)
{
    int base = value.rfind("0x", 0) == 0 ? 16 : 10;

    try {
        parsed = (u32)stoul(value, nullptr, base);
    } catch (...) {
        throw Exception("Invalid argument: " + value);
    }
}

void
Parser::parse(const string &value, double &parsed)
{
    if (value.find_first_not_of("-0123456789.beBE") != std::string::npos) {
        throw Exception("Invalid floating point value: " + value);
    }
    try {
        parsed = stod(value);
    } catch (...) {
        throw Exception("Invalid floating point value: " + value);
    }
}

void
Parser::parse(const string &value, double &parsed, double min, double max)
{
    parse(value, parsed);

    if (parsed < min) {
        throw Exception("Invalid argument. Value must be >= " + std::to_string(min));
    }
    if (parsed > max) {
        throw Exception("Invalid argument. Value must be <= " + std::to_string(max));
    }
}

void
Parser::parse(const string &value, mpf_class &parsed)
{
    try {
        parsed = mpf_class(value);
    } catch (...) {
        throw Exception("Invalid argument: " + value);
    }
}

void
Parser::parse(const string &value, GpuColor &parsed)
{
    std::map <string, GpuColor> modes = {

        { "black",          GpuColor::black     },
        { "white",          GpuColor::white     },
        { "red",            GpuColor::red       },
        { "green",          GpuColor::green     },
        { "blue",           GpuColor::blue      },
        { "yellow",         GpuColor::yellow    },
        { "magenta",        GpuColor::magenta   },
        { "cyan",           GpuColor::cyan      },
        { "transparent",    GpuColor(0xFFFFFFu) }
    };

    if (modes.count(value)) {

        parsed = modes[value];
        return;
    }

    parse(value, parsed.rawValue);
}

void
Parser::parse(const string &value, std::optional<GpuColor> &parsed)
{
    if (value == "") {

        parsed = {};
        return;
    }

    GpuColor color;
    parse(value, color);
    parsed = color;
}

void
Parser::parse(const string &value, ColoringMode &parsed)
{
    std::map <string, ColoringMode> modes = {

        { "classic",    ColoringMode::Classic },
        { "smooth",     ColoringMode::Smooth  }
    };

    try {
        parsed = modes.at(value);
    } catch (...) {
        throw Exception("Unknown coloring mode: '" + value + "'");
    }
}

void
Parser::parse(const string &value, DynamicFloat &parsed)
{
    std::vector<float> xn;
    std::vector<float> yn;

    Time t;
    double y;

    if (auto pos1 = value.find("/"); pos1 == std::string::npos) {

        // A single value is given
        Parser::parse(value, y);
        xn.push_back(0.0);
        yn.push_back(y);

    } else {

        // A list of values is given
        auto pairs = split(value, ',');

        // Split list entries
        for (auto &it : pairs) {

            if (auto pos = it.find("/"); pos != std::string::npos) {

                auto first = it.substr(0, pos);
                auto last = it.substr(pos + 1, std::string::npos);

                Parser::parse(first, t);
                xn.push_back(t.asSeconds());
                Parser::parse(last, y);
                yn.push_back(float(y));

            } else {
                throw Exception("Invalid spline pair: '" + it + "'");
            }
        }
    }

    parsed.init(xn, yn);

    /*
    log::cout << parsed;
    for (double i = 0.0; i <= 70.1; i += 0.5) {
        printf("Spline(%f) = %f\n", i, parsed(i));
    }
    */
}

void
Parser::parse(const string &value, Time &parsed)
{
    auto values = split(value, ':');

    long mm = 0; // Minutes
    long ss = 0; // Seconds
    long tt = 0; // Tenths

    // Expected formats: 'mm:ss' or 'mm:ss:tt'
    if (values.size() < 2 || values.size() > 3) {
        throw Exception("Invalid time specification: " + value);
    }

    parse(values[0], mm);
    parse(values[1], ss);
    if (values.size() == 3) parse(values[2], tt);

    // Minutes and tenth must be in their usual range
    if (mm < 0 || mm > 59) {
        throw Exception(std::to_string(mm) + " is out of range (expected: 0..59)");
    }
    if (tt < 0 || tt > 9) {
        throw Exception(std::to_string(tt) + " is out of range (expected: 0..9)");
    }

    parsed = Time::seconds(60.0f * mm + ss + tt / 10.0f);
}

void
Parser::parse(const string &value, std::pair<isize,isize> &parsed)
{
    isize first = 0;
    isize last = LONG_MAX;

    try {

        if (auto pos2 = value.find("-"); pos2 != std::string::npos) {

            first = std::stol(value.substr(0, pos2));
            last = std::stol(value.substr(pos2 + 1, std::string::npos));

        } else {

            first = std::stol(value);
            last = first;
        }

    } catch (...) {
        throw Exception(value + " is not a valid frame range.");
    }

    parsed = { first, last };
}

void
Parser::ltrim(string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}
void
Parser::rtrim(string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

void
Parser::trim(string &s)
{
    ltrim(s);
    rtrim(s);
}

void
Parser::erase(string &s, char c)
{
    s.erase(std::remove(s.begin(), s.end(), c), s.end());
}

void
Parser::tolower(string &s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char ch){ return std::tolower(ch); });
}

}
