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
    
    std::cout << "DeepDrill Colorizer" << VER_MAJOR << "." << VER_MINOR;
    std::cout << " - (C)opyright 2021 Dirk W. Hoffmann";
    std::cout << std::endl << std::endl;
                
    // Parse all command line arguments
    parseArguments(args, keys);
        
    // Parse all options
    // Options options(keys);
    
    // Create a colorizer
    // Colorizer colorizer(options);
    // colorizer.drill();
}

void
Application::parseArguments(std::vector <string> &args, map<string,string> &keys)
{
    bool hasMapFile = false;
        
    while (!args.empty()) {
                
        // Is the argument an option?
        if (args.front()[0] == '-') { parseOption(args, keys); continue; }

        // If not, it must be a location file
        if (hasMapFile) throw dd::SyntaxError();
        hasMapFile = true;
        
        parseMapFile(args, keys);
    }
    
    if (!hasMapFile) throw dd::SyntaxError();
}

void
Application::parseOption(vector <string> &args, map<string,string> &keys)
{
    auto arg = pop(args);

    if (arg == "-v") {
        
        keys["verbose"] = "1";
        return;
    }
    if (arg == "-p") {
        
        auto path = pop(args);
        if (extractSuffix(path) != "prf") throw dd::SyntaxError();
        Parser::parse(path, keys);
        return;
    }
    if (arg == "-o") {
        
        auto path = pop(args);
        if (extractSuffix(path) != "tiff") throw dd::SyntaxError();
        keys["imgfile"] = path;
        return;
    }

    throw dd::Exception("Unknown option: " + arg);
}

void
Application::parseMapFile(vector <string> &args, map<string,string> &keys)
{
    auto path = pop(args);
    if (extractSuffix(path) != "map") throw dd::SyntaxError();
    keys["mapfile"] = path;
}

string
Application::pop(vector <string> &args)
{
    if (args.empty()) throw dd::SyntaxError();
    
    auto result = args.front();
    args.erase(args.begin());
    
    return result;
}

}
