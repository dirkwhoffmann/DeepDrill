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
#include "DeepDrill.h"
#include "Colorizer.h"
#include "Driller.h"
#include "Logger.h"
#include "Options.h"
#include "Maker.h"
#include <getopt.h>

int main(int argc, char *argv[])
{
    using namespace dd;
    
    try {

        DeepDrill().main(argc, argv);

    } catch (dd::SyntaxError &e) {

        log::cout << "Usage: ";
        log::cout << "deepdrill [-bmv] [-p <profile>] -o <output> <input>" << log::endl;
        log::cout << log::endl;
        log::cout << "       -b or --batch    Run in batch mode" << log::endl;
        log::cout << "       -m or --make     Run the Makefile generator" << log::endl;
        log::cout << "       -v or --verbose  Run in verbose mode" << log::endl;
        log::cout << "       -p or --profile  Customize settings" << log::endl;
        log::cout << log::endl;

        if (!e.description.empty()) {
            log::cout << e.what() << log::endl;
        }

        return 1;

    } catch (std::exception &e) {

        log::cout << e.what() << log::endl;
        return 1;
    }

    return 0;
}

namespace dd {

void
DeepDrill::main(int argc, char *argv[])
{
    map<string,string> keys;
    string option = "";
                        
    // Parse all command line arguments
    parseArguments(argc, argv, keys);

    log::cout << "DeepDrill " << VER_MAJOR << "." << VER_MINOR;
    log::cout << " - (C)opyright Dirk W. Hoffmann";
    log::cout << log::endl << log::endl;

    // Setup the GMP library
    setupGmp(keys);

    // Parse all options
    Options opt(keys);
    
    // Start a stop watch
    Clock stopWatch;

    // Execute
    opt.make ? runMaker(opt) : runPipeline(opt);

    log::cout << log::vspace << "Total time: " << stopWatch.stop() << log::endl;
}

void
DeepDrill::parseArguments(int argc, char *argv[], map<string,string> &keys)
{    
    static struct option long_options[] = {
        
        { "verbose", no_argument,       NULL, 'v' },
        { "make",    no_argument,       NULL, 'm' },
        { "batch",   no_argument,       NULL, 'b' },
        { "profile", required_argument, NULL, 'p' },
        { "output",  required_argument, NULL, 'o' },
        { NULL,      0,                 NULL,  0  }
    };
    
    // Don't print the default error messages
    opterr = 0;
    
    // Remember the path to the DeppDrill executable
    keys["exec"] = makeAbsolutePath(argv[0]);

    // Parse all options
    while (1) {
        
        int arg = getopt_long(argc, argv, ":vmbp:o:", long_options, NULL);
        if (arg == -1) break;

        switch (arg) {
                
            case 'v':
                keys["verbose"] = "1";
                break;
                
            case 'm':
                keys["make"] = "1";
                break;

            case 'b':
                log::cout.setSilent(true);
                break;

            case 'p':
                profiles.push_back(makeAbsolutePath(optarg));
                break;
            
            case 'o':
                outputs.push_back(makeAbsolutePath(optarg));
                break;

            case ':':
                throw SyntaxError("Missing argument for option '" +
                                  string(argv[optind - 1]) + "'");
                
            default:
                throw SyntaxError("Invalid option '" +
                                  string(argv[optind - 1]) + "'");
        }
    }
    
    // Parse all remaining arguments
    while (optind < argc) {
        inputs.push_back(makeAbsolutePath(argv[optind++]));
    }
    
    checkArguments(keys);
    readInputs(keys);
    readOutputs(keys);
    readProfiles(keys);
}

void
DeepDrill::checkArguments(map<string,string> &keys)
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
        if (!fileExists(out)) {
            throw SyntaxError("Output directory " + out + " does not exist");
        }
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
        if (outSuffix != "map" && outSuffix != "tiff" && outSuffix != "png") {
            throw SyntaxError(outputs.front() +
                              ": Invalid format. Expected .map, .tiff, or .png");
        }
    }
}

void
DeepDrill::readInputs(map<string,string> &keys)
{
    auto path = inputs.front();
    auto suffix = extractSuffix(path);

    keys["input"] = path;
    
    if (suffix == "loc") {
        Parser::parse(path, keys);
        return;
    }
    if (suffix == "map") {
        return;
    }
    if (isDirectory(path)) {
        return;
    }

    throw SyntaxError(path + ": Unknown input format");
}

void
DeepDrill::readOutputs(map<string,string> &keys)
{
    auto path = outputs.front();
    auto suffix = extractSuffix(path);

    keys["output"] = path;

    if (suffix == "map") {
        return;
    }
    if (suffix == "tiff" || suffix == "tif" || suffix == "png") {
        return;
    }
    if (isDirectory(path)) {
        return;
    }

    throw SyntaxError(path + ": Unknown output format");
}

void
DeepDrill::readProfiles(map<string,string> &keys)
{
    for (auto &path: profiles) {
        Parser::parse(path, keys);
    }
}

void
DeepDrill::setupGmp(std::map <string,string> &keys)
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

void
DeepDrill::runPipeline(Options &opt)
{
    DrillMap drillMap(opt);

    // Create the drill map
    if (opt.inputFormat == Format::MAP) {
        
        // Load the drill map from disk
        drillMap.load(opt.input);

    } else {
    
        // Run the driller
        Driller driller(opt, drillMap);
        driller.drill();
    }
    
    // Are we supposed to save the map file?
    if (opt.outputFormat == Format::MAP) {
        
        drillMap.save(opt.output);
    }
    
    // Are we suppoed to create an image file?
    if (opt.outputFormat == Format::TIF || opt.outputFormat == Format::PNG) {

        // Run the colorizer
        Colorizer colorizer(opt, drillMap);
        colorizer.colorize();
        colorizer.save(opt.output);
    }
}

void
DeepDrill::runMaker(Options &opt)
{
    Maker maker(opt);
    maker.generate();
}

}
