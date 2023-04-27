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

#include "DeepDrill.h"
#include "Chrono.h"
#include "Colorizer.h"
#include "Driller.h"
#include "Exception.h"
#include "IO.h"
#include "Logger.h"
#include "Maker.h"
#include "Options.h"
#include "Parser.h"
#include "ProgressIndicator.h"

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
        log::cout << "       -b or --batch     Run in batch mode" << log::endl;
        log::cout << "       -m or --make      Run the Makefile generator" << log::endl;
        log::cout << "       -v or --verbose   Run in verbose mode" << log::endl;
        log::cout << "       -a or --assets    Path to assets directory" << log::endl;
        log::cout << "       -p or --profile   Customize settings" << log::endl;
        log::cout << "       -o or --output    Output file" << log::endl;
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
    log::cout << "DeepDrill " << VER_MAJOR << "." << VER_MINOR;
    log::cout << " - (C)opyright Dirk W. Hoffmann";
    log::cout << log::endl << log::endl;

    // Parse command line arguments
    parseArguments(argc, argv);

    // Check arguments for consistency
    checkArguments();

    // Setup the GMP library
    setupGmp();

    // Read files
    readProfiles();
    readInputs();

    // Deduce missing options
    opt.derive();

    // Start a stop watch
    Clock stopWatch;

    // Execute
    opt.flags.make ? runMaker() : runPipeline();

    log::cout << log::vspace << "Total time: " << stopWatch.stop() << log::endl;
}

void
DeepDrill::parseArguments(int argc, char *argv[])
{
    static struct option long_options[] = {
        
        { "verbose",  no_argument,       NULL, 'v' },
        { "make",     no_argument,       NULL, 'm' },
        { "batch",    no_argument,       NULL, 'b' },
        { "assets",   required_argument, NULL, 'a' },
        { "profile",  required_argument, NULL, 'p' },
        { "output",   required_argument, NULL, 'o' },
        { NULL,       0,                 NULL,  0  }
    };

    // Don't print the default error messages
    opterr = 0;
    
    // Remember the path to the DeppDrill executable
    opt.files.exec = makeAbsolutePath(argv[0]);

    // Parse all options
    while (1) {
        
        int arg = getopt_long(argc, argv, ":vmba:p:o:", long_options, NULL);
        if (arg == -1) break;

        switch (arg) {
                
            case 'v':
                opt.flags.verbose = true;
                break;
                
            case 'm':
                opt.flags.make = true;
                break;

            case 'b':
                opt.flags.batch = true;
                break;

            case 'a':
                opt.assets = optarg;
                break;

            case 'p':
                profiles.push_back(optarg);
                break;
            
            case 'o':
                outputs.push_back(optarg);
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
        inputs.push_back(argv[optind++]);
    }
}

void
DeepDrill::checkArguments()
{
    // The user needs to specify a single input
    if (inputs.size() < 1) throw SyntaxError("No input file is given");
    if (inputs.size() > 1) throw SyntaxError("More than one input file is given");

    auto input = inputs.front();
    auto inputFormat = opt.files.inputFormat = getFormat(input);

    // The user needs to specify a single output
    if (outputs.size() < 1) throw SyntaxError("No output file is given");
    if (outputs.size() > 1) throw SyntaxError("More than one output file is given");

    auto output = outputs.front();
    auto outputFormat = opt.files.outputFormat = getFormat(output);

    // All profiles must be files of type ".prf"
    for (auto &it : profiles) {
        if (getFormat(it) != Format::PRF) {
            throw SyntaxError(it + " is not a profile (.prf)");
        }
    }

    if (opt.flags.make) {

        // The input files must be a location files
        if (inputFormat == Format::LOC) {
            opt.files.input = opt.findLocationFile(input);
        } else {
            throw SyntaxError(input + " is not a location file (.loc)");
        }

        // The output must be a directory
        if (inputFormat == Format::DIR) {
            opt.files.output = output;
        } else {
            throw SyntaxError(input + " is not a directory");
        }

    } else {
                
        // The input file must be a location file or a map file
        if (inputFormat == Format::LOC) {
            opt.files.input = opt.findLocationFile(input);
        } else if (inputFormat == Format::MAP) {
            opt.files.input = input;
        } else {
            throw SyntaxError(input + ": Invalid format. Expected .loc or .map");
        }
        
        // The output file must be a map file or an image file
        if (outputFormat == Format::MAP) {
            opt.files.output = output;
        } else if (isImageFormat(outputFormat)) {
            opt.files.output = output;
        } else {
            throw SyntaxError(output + ": Invalid format. Expected .map, .bmp, .jpg, or .png");
        }
    }
}

void
DeepDrill::readInputs()
{
    if (opt.files.inputFormat == Format::LOC) {
        Parser::parse(opt.files.input, [this](string k, string v) { opt.parse(k,v); });
    }
}

void
DeepDrill::readProfiles()
{
    for (auto &profile: profiles) {

        if (auto path = opt.findProfile(profile); path != "") {
            Parser::parse(path, [this](string k, string v) { opt.parse(k,v); });
        } else {
            throw FileNotFoundError(profile);
        }
    }
}

void
DeepDrill::setupGmp()
{
    isize accuracy = 64;

    auto name = inputs.front();
    auto suffix = extractSuffix(name);

    if (suffix == "loc") {

        if (auto path = opt.findLocationFile(name); path != "") {

            /* If a location is given, we need to adjust the GMP precision based
             * on the zoom factor. Because we haven't parsed any input file when
             * this function is called, we need to peek this value directly from
             * the location file.
             */
            Parser::parse(path, [&accuracy](string key, string value) {

                if (key == "location.zoom") {

                    try {

                        long exponent = 0;
                        auto zoom = mpf_class(value);
                        mpf_get_d_2exp(&exponent, zoom.get_mpf_t());
                        accuracy = exponent + 64;
                        return;

                    } catch (...) { }
                }
            });
        }
    }

    setupGmp(accuracy);
}

void
DeepDrill::setupGmp(isize accuracy)
{
    mpf_set_default_prec(accuracy);
}

void
DeepDrill::runPipeline()
{
    DrillMap drillMap(opt);

    // Create the drill map
    if (opt.files.inputFormat == Format::MAP) {
        
        // Load the drill map from disk
        drillMap.load(opt.files.input);

    } else {

        BatchProgressIndicator progress(opt, "Drilling", opt.files.output);

        // Run the driller
        Driller driller(opt, drillMap);
        driller.drill();

        // Are we supposed to save the map file?
        if (opt.files.outputFormat == Format::MAP) {

            drillMap.save(opt.files.output);
        }
    }

    // Are we suppoed to create an image file?
    if (isImageFormat(opt.files.outputFormat)) {

        BatchProgressIndicator progress(opt, "Colorizing", opt.files.output);

        // Run the colorizer
        Colorizer colorizer(opt, drillMap);
        colorizer.colorize();
        colorizer.save(opt.files.output, opt.files.outputFormat);
    }
}

void
DeepDrill::runMaker()
{
    Maker maker(opt);
    maker.generate();
}

}
