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
    log::cout << "DeepDrill " << VER_MAJOR << "." << VER_MINOR;
    log::cout << " - (C)opyright Dirk W. Hoffmann";
    log::cout << log::endl << log::endl;

    // Parse command line arguments
    parseArguments(argc, argv);

    // Check arguments for consistency
    checkArguments();

    // Setup the GMP library
    setupGmp();

    // Process files
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
        { "profile",  required_argument, NULL, 'p' },
        { "output",   required_argument, NULL, 'o' },
        { NULL,       0,                 NULL,  0  }
    };

    // Don't print the default error messages
    opterr = 0;
    
    // Remember the path to the DeppDrill executable
    opt.exec = makeAbsolutePath(argv[0]);

    // Parse all options
    while (1) {
        
        int arg = getopt_long(argc, argv, ":vmbp:o:", long_options, NULL);
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
    auto inFormat = getFormat(in);
    auto outFormat = getFormat(out);

    // auto inSuffix = extractSuffix(in);
    // auto outSuffix = extractSuffix(out);

    // All profiles must be files of type ".prf"
    for (auto &it : profiles) {
        if (getFormat(it) != Format::PRF) {
            throw SyntaxError(it + " is not a profile (.prf)");
        }
    }
    
    if (opt.make) {
        
        // The input files must be a location files
        if (inFormat != Format::LOC) {
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
            throw Exception("The output directory must be empty.");
        }
        
    } else {
                
        // The input file must be a location file or a map file
        if (inFormat != Format::LOC && inFormat != Format::MAP) {
            throw SyntaxError(inputs.front() +
                              ": Invalid format. Expected .loc or .map");
        }
        
        // The output file must be a map file or an image file
        if (outFormat != Format::MAP && !isImageFormat(outFormat)) {
            throw SyntaxError(outputs.front() +
                              ": Invalid format. Expected .map, .bmp, .jpg, or .png");
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
    auto format = getFormat(path);

    opt.output = path;

    if (format == Format::MAP) {
        return;
    }
    if (isImageFormat(format)) {
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
DeepDrill::setupGmp()
{
    isize accuracy = 64;

    if (extractSuffix(inputs.front()) == "loc") {

        /* If a location is given, we need to adjust the GMP precision based
         * on the zoom factor. Because we haven't parsed any input file when
         * this function is called, we need to peek this value directly from
         * the location file.
         */
        Parser::parse(inputs.front(), [&accuracy](string key, string value) {

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
    if (opt.inputFormat == Format::MAP) {
        
        // Load the drill map from disk
        drillMap.load(opt.input);

    } else {

        BatchProgressIndicator progress(opt, "Drilling", opt.output);

        // Run the driller
        Driller driller(opt, drillMap);
        driller.drill();
    }
    
    // Are we supposed to save the map file?
    if (opt.outputFormat == Format::MAP) {
        
        drillMap.save(opt.output);
    }
    
    // Are we suppoed to create an image file?
    if (opt.outputFormat == Format::BMP ||
        opt.outputFormat == Format::JPG ||
        opt.outputFormat == Format::PNG) {

        BatchProgressIndicator progress(opt, "Colorizing", opt.output);

        // Run the colorizer
        Colorizer colorizer(opt, drillMap);
        colorizer.colorize();
        colorizer.save(opt.output, opt.outputFormat);
    }
}

void
DeepDrill::runMaker()
{
    Maker maker(opt);
    maker.generate();
}

}
