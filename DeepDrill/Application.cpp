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

#include "Application.h"
#include "Driller.h"
#include "IO.h"
#include "Options.h"
#include "Parser.h"
#include <iostream>

namespace dd {

void
Application::main(std::vector <string> &args)
{
    map<string,string> keys;
    string option = "";
    
    std::cout << "Deep Drill " << VER_MAJOR << "." << VER_MINOR;
    std::cout << " - (C)opyright 2021 Dirk W. Hoffmann";
    std::cout << std::endl << std::endl;
        
    // Display a syntax descriptions if no arguments are given
    if (args.empty()) throw dd::SyntaxError();
        
    // Parse all command line arguments
    while (!args.empty()) {
                
        auto arg = args.front();

        if (isOption(arg)) { parseOption(args, keys); continue; }
        if (isProfile(arg)) { parseProfile(args, keys); continue; }

        throw dd::SyntaxError();
    }

    // Read the location file
    readLocationFile(keys);
    
    // Setup the GMP library
    setupGmp(keys);

    // Parse all options
    Options options(keys);
    
    // Create a driller
    Driller driller(options);
    driller.drill();
}

bool
Application::isOption(const string &s)
{
    if (s[0] != '-') return false;
        
    if (s == "-o") return true;
    if (s == "-v") return true;
        
    return false;
}

bool
Application::isProfile(const string &s)
{
    return extractSuffix(s) == "prf";
}

void
Application::parseOption(vector <string> &args, map<string,string> &keys)
{
    if (args.front() == "-v") {

        args.erase(args.begin());
        keys["verbose"] = "1";
        return;
    }
    
    if (args.front() == "-o") {
        
        args.erase(args.begin());
        if (args.empty()) throw Exception("Option -o has no argument");
        keys["mapfile"] = args.front();
        args.erase(args.begin());
        return;
    }
    
    throw Exception("Unknown option: " + args.front());
}

void
Application::parseProfile(vector <string> &args, map<string,string> &keys)
{
    Parser::parse(args.front(), keys);
    args.erase(args.begin());
}

void
Application::readLocationFile(map<string,string> &keys)
{
    // Lookup the mapfile
    auto it = keys.find("mapfile");
    if (it == keys.end()) throw SyntaxError();
    
    // Assemble the path of the location file
    auto path = stripSuffix(it->second) + ".loc";
    
    // Read the file
    Parser::parse(path, keys);
}

void
Application::setupGmp(std::map <string,string> &keys)
{
    long exponent = 0;
    
    // Peek the zoom factor
    if (auto it = keys.find("location.zoom"); it != keys.end()) {
        
        try {
            auto zoom = mpf_class(it->second);
            mpf_get_d_2exp(&exponent, zoom.get_mpf_t());
        } catch (...) { }
        
        mpf_set_default_prec(exponent + 64);
    }
}

}
