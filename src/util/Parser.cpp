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
Parser::parse(const string &path, std::function<void(string,string)>callback, isize nr)
{
    auto name = extractName(path);
    auto fs = std::ifstream(path);

    if (!fs.is_open()) {
        throw Exception("Failed to open file " + path + ".");
    }
 
    try { parse(fs, callback, nr); } catch (ParseError &e) {

        e.file = name;
        throw;
    }
}

void
Parser::parse(std::ifstream &stream, std::function<void(string,string)>callback, isize nr)
{
    std::stringstream ss;
    ss << stream.rdbuf();
    
    parse(ss, callback, nr);
}

void
Parser::parse(std::stringstream &stream, std::function<void(string,string)>callback, isize nr)
{
    isize line = 0;
    string input;
    string section;
    
    while(std::getline(stream, input)) {

        line++;

        // Remove comments
        input = input.substr(0, input.find("#"));

        // Remove white spaces
        input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
        // trim(input);
                
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

            isize start = 0;
            isize stop = LONG_MAX;

            // Check if the key-value pair has a prefix
            if (auto pos2 = key.find(":"); pos2 != std::string::npos) {

                auto prefix = key.substr(0, pos2);
                key = key.substr(pos2 + 1, std::string::npos);

                try {

                    // Check if the prefix specifies a range
                    if (auto pos3 = prefix.find("-"); pos3 != std::string::npos) {

                        start = stol(prefix.substr(0, pos3));
                        stop = stol(prefix.substr(pos3 + 1, std::string::npos));


                    } else {

                        start = stol(prefix);
                        stop = start;
                    }

                } catch (...) {
                    throw ParseError(Exception("Invalid prefix: " + prefix), line);
                }
            }

            // Perform the range check
            if (nr < start || nr > stop) continue;

            // Remove white spaces
            // trim(key);
            // trim(value);
            
            // Convert the key to lower case
            tolower(key);
            
            // Process the key-value pair
            try {
                callback(section + "." + key ,value);
            } catch (const Exception &e) {
                throw ParseError(e, line);
            }
            continue;
        }
        
        throw ParseError(SyntaxError("Parse error"), line);
    }
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
Parser::tolower(string &s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char ch){ return std::tolower(ch); });
}

}
