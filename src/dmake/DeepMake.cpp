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

#include "DeepMake.h"
#include "Maker.h"

#include <getopt.h>

int main(int argc, char *argv[])
{
    return dd::DeepMake().main(argc, argv);
}

namespace dd {

void
DeepMake::syntax()
{
    log::cout << "Usage: ";
    log::cout << "deepmake [-a <path>] [-p <profile>] -o <output> <input>" << log::endl;
    log::cout << log::endl;
    log::cout << "       -a or --assets    Optional path to asset files" << log::endl;
    log::cout << "       -p or --profile   Customize settings" << log::endl;
    log::cout << "       -o or --output    Output file" << log::endl;
}

void
DeepMake::parseArguments(int argc, char *argv[])
{
    static struct option long_options[] = {

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

        int arg = getopt_long(argc, argv, ":a:p:o:", long_options, NULL);
        if (arg == -1) break;

        switch (arg) {

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
DeepMake::checkCustomArguments()
{
    // The user needs to specify a single output
    if (outputs.size() < 1) throw SyntaxError("No output file is given");
    if (outputs.size() > 1) throw SyntaxError("More than one output file is given");
    if (!outputs.empty()) opt.files.output = outputs.front();

    // The input must be a location file
    opt.files.input = assets.findAsset(opt.files.input , Format::LOC);

    // The output must be an existing directory
    opt.files.output = assets.findAsset(opt.files.output, Format::DIR);
}

void
DeepMake::run()
{
    // Ask for permission if many files would be created
    auto numFiles = opt.video.keyframes + 3;
    if (opt.video.keyframes > 100) {

        log::cout << numFiles << " will be created. Do you want to proceed [no]? ";
        string line; std::getline(std::cin, line);

        if (line != "y" && line != "yes") {

            log::cout << "Aborting" << log::endl;
            return;
        }
        log::cout << log::endl;
    }

    Maker(opt).generate();
}

}

