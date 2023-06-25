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

                // Get the range of keyframes this key-value pair applies to
                auto range = stripRange(key);

                if (nr >= range.first && nr <= range.second) {

                    // Process the key-value pair
                    callback(section + "." + key ,value);
                }
                continue;
            }

            throw SyntaxError("Parse error");
        }

    } catch (Exception &e) {

        throw ParseError(e, line);
    }
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
