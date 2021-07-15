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
#include "Colorizer.h"
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
                    
    // Parse all command line arguments
    parseArguments(args, keys);

    readInputs(keys);
    readOutputs(keys);
    readProfiles(keys);
    
    std::cout << "DeepDrill " << VER_MAJOR << "." << VER_MINOR;
    std::cout << " - (C)opyright 2021 Dirk W. Hoffmann";
    std::cout << std::endl << std::endl;
        
    // Setup the GMP library
    setupGmp(keys);

    // Parse all options
    Options opt(keys);

    std::cout << "mapFileIn" << opt.mapFileIn << std::endl;
    std::cout << "mapFileOut" << opt.mapFileOut << std::endl;
    std::cout << "tifFileOut" << opt.tifFileOut << std::endl;

    // Is the input a map file? If yes, load it from disk
    if (opt.mapFileIn != "") drillMap.load(opt.mapFileIn);
    
    // If not, compute the drill map
    if (opt.mapFileIn == "") {
        
        // Run the driller
        Driller driller(opt, drillMap);
        driller.drill();
    }
    
    // Are we supposed to save the map file?
    if (opt.mapFileOut != "") drillMap.save(opt.mapFileOut);
    
    // Are we suppoed to create an image file?
    if (opt.tifFileOut != "") {

        // Run the colorizer
        Colorizer colorizer(opt, drillMap);
        colorizer.colorize();
        colorizer.save(opt.tifFileOut);
    }
}

void
Application::parseArguments(std::vector <string> &args, map<string,string> &keys)
{
    if (args.empty()) throw SyntaxError();
    
    while (!args.empty()) {
        
        if (args.front()[0] == '-') {
            
            parseOption(args, keys);
            continue;
        }

        inputs.push_back(makeAbsolutePath(pop(args)));
    }
    
    checkArguments(keys);
}

void
Application::parseOption(vector <string> &args, map<string,string> &keys)
{
    auto arg = pop(args);

    if (arg == "-v" || arg == "-verbose") {
        
        keys["verbose"] = "1";
        return;
    }
    if (arg == "-m" || arg == "-make") {
        
        keys["make"] = "1";
        return;
    }
    if (arg == "-p" || arg == "-profile") {
        
        profiles.push_back(pop(args));
        return;
    }
    if (arg == "-o" || arg == "-output") {

        outputs.push_back(pop(args));
        return;
    }

    throw SyntaxError("Unknown option: " + arg);
}

string
Application::pop(vector <string> &args)
{
    if (args.empty()) throw SyntaxError("Missing argument");
    
    auto result = args.front();
    args.erase(args.begin());
    
    return result;
}

void
Application::checkArguments(map<string,string> &keys)
{
    // The user needs to specify a single input
    if (inputs.size() < 1) throw SyntaxError("No input file is given");
    if (inputs.size() > 1) throw SyntaxError("More than one input file is given");
    
    // The user needs to specify a single output
    if (outputs.size() < 1) throw SyntaxError("No output file is given");
    if (outputs.size() > 1) throw SyntaxError("More than one output file is given");

    auto in = inputs.front();
    auto out = outputs.front();
    auto inSuffix = extractSuffix(in);
    auto outSuffix = extractSuffix(out);

    // All profiles must be files of type ".prf"
    for (auto &it : profiles) {
        if (extractSuffix(it) != "prf") {
            throw SyntaxError(it + " is not a profile (.prf)");
        }
    }
    
    if (keys.find("make") != keys.end()) {
        
        // The input files must be a location files
        if (inSuffix != "loc") {
            throw SyntaxError(in + " is not a location file (.loc)");
        }

        // The output must be an empty directory
        if (!isDirectory(out)) {
            throw SyntaxError("The output file must be a directory");
        }
        if (numDirectoryItems(out) > 0) {
            throw SyntaxError("The output directory must be empty");
        }
        
    } else {
                
        // The input file must be a location file or a map file
        if (inSuffix != "loc" && outSuffix != "map") {
            throw SyntaxError(inputs.front() +
                              ": Invalid format. Expected .loc or .map");
        }
        
        // The output file must be a map file or an image file
        if (outSuffix != "map" && outSuffix != "tiff") {
            throw SyntaxError(outputs.front() +
                              ": Invalid format. Expected .map or .tiff");
        }
    }
}

void
Application::readInputs(map<string,string> &keys)
{
    auto path = inputs.front();
    auto suffix = extractSuffix(path);

    if (suffix == "map") {
        
        keys["mapfilein"] = path;
        return;
    }
    if (suffix == "loc") {
        
        Parser::parse(path, keys);
        return;
    }
    if (isDirectory(path)) {
        
        keys["projectdir"] = path;
        return;
    }

    throw SyntaxError(path + ": Invalid input format");
}

void
Application::readOutputs(map<string,string> &keys)
{
    auto path = outputs.front();
    auto suffix = extractSuffix(path);

    if (suffix == "map") {
        
        keys["mapfileout"] = path;
        return;
    }
    if (suffix == "tiff" || suffix == "tif") {
        
        keys["tiff"] = path;
        return;
    }
    if (isDirectory(path)) {
        
        keys["outdir"] = path;
        return;
    }

    throw SyntaxError(path + ": Invalid output format");
}

void
Application::readProfiles(map<string,string> &keys)
{
    for (auto &path: profiles) {
        Parser::parse(path, keys);
    }
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
