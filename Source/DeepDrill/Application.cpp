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
                
    // Parse all command line arguments
    parseArguments(args, keys);
        
    // Setup the GMP library
    setupGmp(keys);

    // Parse all options
    Options options(keys);
    
    // Create a driller
    Driller driller(options);
    driller.drill();
}

void
Application::parseArguments(std::vector <string> &args, map<string,string> &keys)
{
    bool hasLocationFile = false;
        
    while (!args.empty()) {
                
        auto arg = args.front();
        
        if (arg == "-v") { parseVerbose(args, keys); continue; }
        if (arg == "-p") { parseProfile(args, keys); continue; }
        if (arg == "-o") { parseOutFile(args, keys); continue; }
        if (arg[0] == '-') throw Exception("Unknown option: " + arg);
        
        parseInFile(args, keys);
        hasLocationFile = true;
    }
    
    if (!hasLocationFile) throw SyntaxError();
}

void
Application::parseVerbose(vector <string> &args, map<string,string> &keys)
{
    args.erase(args.begin());
    keys["verbose"] = "1";
}

void
Application::parseProfile(vector <string> &args, map<string,string> &keys)
{
    if (args.size() < 2) throw SyntaxError();

    args.erase(args.begin());
    auto path = args.front();
    auto suffix = extractSuffix(path);
    args.erase(args.begin());

    if (suffix != "prf") throw SyntaxError();
    Parser::parse(path, keys);
}

void
Application::parseOutFile(vector <string> &args, map<string,string> &keys)
{
    if (args.size() < 2) throw SyntaxError();
    
    args.erase(args.begin());
    auto path = args.front();
    auto suffix = extractSuffix(path);
    args.erase(args.begin());

    if (suffix != "map") throw SyntaxError();
    keys["mapfile"] = args.front();
}

void
Application::parseInFile(vector <string> &args, map<string,string> &keys)
{
    static bool hasLocationFile = false;
    if (hasLocationFile) throw SyntaxError();
    hasLocationFile = true;

    auto path = args.front();
    auto suffix = extractSuffix(path);
    args.erase(args.begin());

    if (suffix != "loc") throw SyntaxError();
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
