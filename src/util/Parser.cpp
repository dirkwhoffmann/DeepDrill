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

        { "black",      GpuColor::black     },
        { "white",      GpuColor::white     },
        { "red",        GpuColor::red       },
        { "green",      GpuColor::green     },
        { "blue",       GpuColor::blue      },
        { "yellow",     GpuColor::yellow    },
        { "magenta",    GpuColor::magenta   },
        { "cyan",       GpuColor::cyan      }
    };

    try {
        parsed = modes.at(value);
    } catch (...) {
        throw Exception("Invalid argument: " + value);
    }
}

void
Parser::parse(const string &value, ColoringMode &parsed)
{
    std::map <string, ColoringMode> modes = {

        { "default", ColoringMode::Default }
    };

    try {
        parsed = modes.at(value);
    } catch (...) {
        throw Exception("Unknown coloring mode: '" + value + "'");
    }
}

void
Parser::parse(const string &value, Dynamic<float> &parsed)
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

                // printf("'%s' / '%s'\n", first.c_str(), last.c_str());
                Parser::parse(first, t);
                Parser::parse(last, y);

                // printf("%f -> %f\n", t.asSeconds(), y);
                xn.push_back(t.asSeconds());
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
    if (auto pos = value.find(":"); pos != std::string::npos) {
        
        auto mm = value.substr(0, pos);
        auto ss = value.substr(pos + 1, std::string::npos);
        
        auto m = std::stol(mm);
        auto s = std::stol(ss);
        
        parsed = Time::seconds(i64(60 * m + s));
    } 
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

std::pair<isize, isize>
Parser::getRange(string &key, bool strip)
{
    isize first = 0;
    isize last = LONG_MAX;

    if (auto pos1 = key.find(":"); pos1 != std::string::npos) {

        try {

            auto range = key.substr(0, pos1);
            if (auto pos2 = range.find("-"); pos2 != std::string::npos) {

                first = std::stol(range.substr(0, pos2));
                last = std::stol(range.substr(pos2 + 1, std::string::npos));

            } else {

                first = std::stol(range);
                last = first;
            }

        } catch (...) {
            throw Exception(key + " is not a valid frame range.");
        };

        if (strip) {
            key = key.substr(pos1 + 1, std::string::npos);
        }
    }

    return { first, last };
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
