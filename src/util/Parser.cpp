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
Parser::parse(const string &path, std::function<void(string,string)>callback)
{
    auto name = extractName(path);
    auto fs = std::ifstream(path);

    if (!fs.is_open()) {
        throw Exception("Failed to open file " + path);
    }
 
    try { parse(fs, callback); } catch (Exception &e) {
        throw Exception("Error in '" + name + "' line " + std::to_string(e.data) + ": " + e.what());
    }
}

void
Parser::parse(std::ifstream &stream, std::function<void(string,string)>callback)
{
    std::stringstream ss;
    ss << stream.rdbuf();
    
    parse(ss, callback);
}

void
Parser::parse(std::stringstream &stream, std::function<void(string,string)>callback)
{
    isize line = 0;
    string input;
    string section;
    
    while(std::getline(stream, input)) {

        line++;
        
        // Remove white spaces
        trim(input);
                
        // Ignore empty lines
        if (input == "") continue;
        
        // Ignore comments
        if (input.substr(0,1) == "#") continue;
                
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
            
            // Remove white spaces
            trim(key);
            trim(value);
            
            // Convert the key to lower case
            tolower(key);
            
            // Add the key-value pair
            callback(section + "." + key ,value);
            // keys[section + "." + key] = value;
            continue;
        }
        
        throw Exception("Syntax error", line);
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
