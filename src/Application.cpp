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
#include "Options.h"
#include "Maker.h"

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

    log::cout << "DeepDrill " << VER_MAJOR << "." << VER_MINOR;
    log::cout << " - (C)opyright 2021 Dirk W. Hoffmann";
    log::cout << log::endl << log::endl;

    // Setup the GMP library
    setupGmp(keys);

    // Parse all options
    Options opt(keys);
    
    // Start a stop watch
    Clock stopWatch;

    // Check the operation mode
    if (keys.find("make") != keys.end()) {
        runMaker(keys, opt);
    } else {
        runPipeline(opt);
    }

    log::cout << log::vspace << "Total time: " << stopWatch.stop() << log::endl;
}

void
Application::runPipeline(Options &opt)
{
    DrillMap drillMap(opt);

    std::cout << "Input format = " << (int)opt.inputFormat << std::endl;
    std::cout << "Output format = " << (int)opt.outputFormat << std::endl;
    std::cout << "Input = " << opt.input << std::endl;
    std::cout << "Output = " << opt.output << std::endl;

    if (opt.inputFormat == Format::MAP) {
        
        // Load the drill map from disk
        drillMap.load(opt.input);

    } else {
    
        // Compute the drill map
        Driller driller(opt, drillMap);
        driller.drill();
    }
    
    // Are we supposed to save the map file?
    if (opt.outputFormat == Format::MAP) {
        
        drillMap.save(opt.output);
    }
    
    // Are we suppoed to create an image file?
    if (opt.outputFormat == Format::TIF) {

        // Run the colorizer
        Colorizer colorizer(opt, drillMap);
        colorizer.colorize();
        colorizer.save(opt.output);
    }
}

void
Application::runMaker(map<string,string> &keys, Options &opt)
{
    Maker maker(keys, opt);
    maker.generate();
}

void
Application::parseArguments(std::vector <string> &args, map<string,string> &keys)
{
    // The first argument is the path the executed binary
    keys["exec"] = pop(args);
    
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

        outputs.push_back(makeAbsolutePath(pop(args)));
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
        if (inSuffix != "loc" && inSuffix != "map") {
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
        
        keys["input"] = path;
        return;
    }
    if (suffix == "loc") {
        
        keys["input"] = path;
        Parser::parse(path, keys);
        return;
    }
    if (isDirectory(path)) {
        
        keys["input"] = path;
        return;
    }

    throw SyntaxError(path + ": Unknown input format");
}

void
Application::readOutputs(map<string,string> &keys)
{
    auto path = outputs.front();
    auto suffix = extractSuffix(path);

    if (suffix == "map") {
        
        keys["output"] = path;
        return;
    }
    if (suffix == "tiff" || suffix == "tif") {
        
        keys["output"] = path;
        keys["tiff"] = path;  // DEPRECATED
        return;
    }
    if (isDirectory(path)) {
        
        keys["output"] = path;
        keys["targetdir"] = path; // DEPRECATED
        return;
    }

    throw SyntaxError(path + ": Unknown output format");
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