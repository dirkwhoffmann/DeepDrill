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
#include "Exception.h"
#include "Logger.h"
#include "Parser.h"
#include "Driller.h"
#include "Colorizer.h"
#include "Maker.h"
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
        log::cout << "       -a or --assets    Optional path to asset files" << log::endl;
        log::cout << "       -p or --profile   Customize settings" << log::endl;
        log::cout << "       -o or --output    Output file" << log::endl;
        log::cout << log::endl;

        log::cout << e << log::endl;
        return 1;

    } catch (Exception &e) {

        log::cout << e << log::endl;
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
    log::cout << "DeepDrill " << Options::version();
    log::cout << " - (C)opyright Dirk W. Hoffmann";
    log::cout << log::endl << log::endl;

    // Parse command line arguments
    parseArguments(argc, argv);

    // Check arguments for consistency
    checkArguments();

    // Setup the GMP library
    setupGmp();

    // Read input files
    readInputs();

    // Customize settings
    readProfiles();

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
                assets.addSearchPath(optarg);
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

    // The user needs to specify a single output
    if (outputs.size() < 1) throw SyntaxError("No output file is given");
    if (outputs.size() > 1) throw SyntaxError("More than one output file is given");

    opt.files.input = inputs.front();
    opt.files.output = outputs.front();

    // All profiles must exist
    for (auto &it : profiles) (void)assets.findAsset(it, Format::PRF);

    if (opt.flags.make) {

        // The input must be a location file
        opt.files.input = assets.findAsset(opt.files.input , Format::LOC);

        // The output must be a existent directory
        opt.files.output = assets.findAsset(opt.files.output, Format::DIR);

    } else {
                
        // The input file must be a location file or a map file
        opt.files.input = assets.findAsset(opt.files.input, { Format::LOC, Format::MAP });

        // The output file must be a map file or an image file
        AssetManager::assureFormat(opt.files.output, { Format::MAP, Format::BMP, Format::JPG, Format::PNG });
    }
}

void
DeepDrill::readInputs()
{
    if (AssetManager::getFormat(opt.files.input) == Format::LOC) {

        Parser::parse(opt.files.input,
                      [this](string k, string v) { opt.parse(k,v); });
    }
}

void
DeepDrill::readProfiles()
{
    for (auto &profile: profiles) {

        Parser::parse(assets.findAsset(profile, Format::PRF),
                      [this](string k, string v) { opt.parse(k,v); });
    }
}

void
DeepDrill::setupGmp()
{
    isize accuracy = 64;

    auto name = inputs.front();
    auto suffix = extractSuffix(name);

    if (AssetManager::getFormat(opt.files.input) == Format::LOC) {

        /* If a location is given, we need to adjust the GMP precision based
         * on the zoom factor. Because we haven't parsed any input file when
         * this function is called, we need to peek this value directly from
         * the location file.
         */
        Parser::parse(opt.files.input, [&accuracy](string key, string value) {

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

    mpf_set_default_prec(accuracy);
}

void
DeepDrill::runPipeline()
{
    auto inputFormat = AssetManager::getFormat(opt.files.input);
    auto outputFormat = AssetManager::getFormat(opt.files.output);

    DrillMap drillMap(opt);

    // Create the drill map
    if (inputFormat == Format::MAP) {

        // Load the drill map from disk
        drillMap.load(opt.files.input);

    } else {

        BatchProgressIndicator progress(opt, "Drilling", opt.files.output);

        // Run the driller
        Driller driller(opt, drillMap);
        driller.drill();

        // Are we supposed to save the map file?
        if (outputFormat == Format::MAP) {

            drillMap.save(opt.files.output);
        }
    }

    // Are we suppoed to create an image file?
    if (AssetManager::isImageFormat(outputFormat)) {

        BatchProgressIndicator progress(opt, "Colorizing", opt.files.output);

        // Run the colorizer
        Colorizer colorizer(opt, drillMap);
        colorizer.colorize();
        colorizer.save(opt.files.output, outputFormat);
    }
}

void
DeepDrill::runMaker()
{
    Maker maker(opt);
    maker.generate();
}

}
