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
#include "Driller.h"
#include "ColorMap.h"
#include "ProgressIndicator.h"

#include <getopt.h>

int main(int argc, char *argv[])
{
    return dd::DeepDrill().main(argc, argv);
}

namespace dd {

void
DeepDrill::syntax()
{
    log::cout << "Usage: ";
    log::cout << "deepdrill [-bv] [-a <path>] [-p <profile>] -o <output> <input>" << log::endl;
    log::cout << log::endl;
    log::cout << "       -b or --batch     Run in batch mode" << log::endl;
    log::cout << "       -v or --verbose   Run in verbose mode" << log::endl;
    log::cout << "       -a or --assets    Optional path to asset files" << log::endl;
    log::cout << "       -p or --profile   Customize settings" << log::endl;
    log::cout << "       -o or --output    Output file" << log::endl;
}

void
DeepDrill::parseArguments(int argc, char *argv[])
{
    static struct option long_options[] = {
        
        { "verbose",  no_argument,       NULL, 'v' },
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
        
        int arg = getopt_long(argc, argv, ":vba:p:o:", long_options, NULL);
        if (arg == -1) break;

        switch (arg) {
                
            case 'v':
                opt.flags.verbose = true;
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
DeepDrill::checkCustomArguments()
{
    // The user needs to specify an output file
    if (outputs.size() < 1) throw SyntaxError("No output file is given");

    // The input file must be a location file or a map file
    opt.files.input = assets.findAsset(opt.files.input, { Format::LOC, Format::MAP });

    // The output file must be a map file or an image file
    AssetManager::assureFormat(opt.files.output, { Format::MAP, Format::BMP, Format::JPG, Format::PNG });
}

void
DeepDrill::run()
{
    auto inputFormat = AssetManager::getFormat(opt.files.input);
    auto outputFormat = AssetManager::getFormat(opt.files.output);

    drillMap.resize();

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
        colorizer.compute(drillMap);
        colorizer.save(opt.files.output, outputFormat);
    }
}

}
