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
        log::cout << "deepdrill [-bmv] [-a <bits>] [-p <profile>] -o <output> <input>" << log::endl;
        log::cout << log::endl;
        log::cout << "       -b or --batch     Run in batch mode" << log::endl;
        log::cout << "       -m or --make      Run the Makefile generator" << log::endl;
        log::cout << "       -v or --verbose   Run in verbose mode" << log::endl;
        log::cout << "       -a or --accuracy  Precision in binary digits" << log::endl;
        log::cout << "       -p or --profile   Customize settings" << log::endl;
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
    // Parse command line arguments
    parseArguments(argc, argv);

    if (opt.batch) {

        log::cout.setSilent(true);

    } else {

        log::cout << "DeepDrill " << VER_MAJOR << "." << VER_MINOR;
        log::cout << " - (C)opyright Dirk W. Hoffmann";
        log::cout << log::endl << log::endl;
    }

    // Read files
    readProfiles();
    readInputs();
    readOutputs();

    // Deduce missing options
    opt.derive();

    // Start a stop watch
    Clock stopWatch;

    // Execute
    opt.make ? runMaker() : runPipeline();

    log::cout << log::vspace << "Total time: " << stopWatch.stop() << log::endl;
}

void
DeepDrill::parseArguments(int argc, char *argv[])
{
    static struct option long_options[] = {
        
        { "verbose",  no_argument,       NULL, 'v' },
        { "make",     no_argument,       NULL, 'm' },
        { "batch",    no_argument,       NULL, 'b' },
        { "accuracy", required_argument, NULL, 'a' },
        { "profile",  required_argument, NULL, 'p' },
        { "output",   required_argument, NULL, 'o' },
        { NULL,       0,                 NULL,  0  }
    };

    // Default accuracy (number of binary digits)
    isize accuracy = 128;

    // Don't print the default error messages
    opterr = 0;
    
    // Remember the path to the DeppDrill executable
    opt.exec = makeAbsolutePath(argv[0]);

    // Parse all options
    while (1) {
        
        int arg = getopt_long(argc, argv, ":vmba:p:o:", long_options, NULL);
        if (arg == -1) break;

        switch (arg) {
                
            case 'v':
                opt.verbose = true;
                break;
                
            case 'm':
                opt.make = true;
                break;

            case 'b':
                opt.batch = true;
                break;

            case 'a':
                accuracy = std::stoi(optarg);
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
    
    checkArguments();
    setupGmp(accuracy);
}

void
DeepDrill::checkArguments()
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
    
    if (opt.make) {
        
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
            throw Exception("The output directory must be empty");
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
DeepDrill::readInputs()
{
    auto path = inputs.front();
    auto suffix = extractSuffix(path);

    opt.input = path;
    
    if (suffix == "loc") {
        Parser::parse(path, [this](string k, string v) { opt.parse(k,v); });
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
DeepDrill::readOutputs()
{
    auto path = outputs.front();
    auto suffix = extractSuffix(path);

    opt.output = path;

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
DeepDrill::readProfiles()
{
    for (auto &path: profiles) {
        Parser::parse(path, [this](string k, string v) { opt.parse(k,v); });
    }
}

void
DeepDrill::setupGmp(isize accuracy)
{
    mpf_set_default_prec(accuracy);
}

/*
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
*/

void
DeepDrill::runPipeline()
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

        // Print a progress status line in batch mode
        if (opt.batch) printProgress();
    }
}

void
DeepDrill::runMaker()
{
    Maker maker(opt);
    maker.generate();
}

void
DeepDrill::printProgress()
{
    std::stringstream ss;

    auto cnt1 = countFiles("png");
    auto cnt2 = countFiles("loc");
    auto percent = isize(100.0 * cnt1 / cnt2);

    // Prepare output string
    Logger logger(ss);
    logger << "\r" << std::string(80, ' ') << "\r";
    logger << log::blue << "[" << percent << "%] ";
    logger << log::blue << cnt1 << "/" << cnt2 << ": ";
    logger << log::green << opt.output;
    logger << log::black << log::flush;

    // Print output in a single chunk
    std::cout << ss.str();
}

}
