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
                opt.files.profiles.push_back(optarg);
                break;

            case 'o':
                opt.files.outputs.push_back(optarg);
                break;

            case ':':
                throw SyntaxError("Missing argument for option '" +
                                  string(argv[optind - 1]) + "'");

            default:
                throw SyntaxError("Invalid option '" +
                                  string(argv[optind - 1]) + "'");
        }
    }

    // Parse remaining arguments
    while (optind < argc) {

        string arg = argv[optind++];

        if (arg.find('=') != std::string::npos) {
            opt.overrides.push_back(arg);
        } else {
            opt.files.inputs.push_back(arg);
        }
    }
}

void
DeepMake::checkCustomArguments()
{
    // The user needs to specify a single output
    if (opt.files.outputs.size() < 1) throw SyntaxError("No output file is given");
    if (opt.files.outputs.size() > 1) throw SyntaxError("More than one output file is given");

    // The input must be a location file
    (void)assets.findAsset(opt.files.inputs.front() , Format::LOC);

    // The output must be an existing directory
    (void)assets.findAsset(opt.files.outputs.front(), Format::DIR);
}

void
DeepMake::run()
{
    // Determine the number of computed files
    auto numFiles = opt.video.keyframes + 4;

    // Ask for permisson
    log::cout << numFiles << " files will be created. Do you want to proceed [no]? ";
    string line; std::getline(std::cin, line);

    if (line != "y" && line != "yes") {

        log::cout << "Aborting" << log::endl;
        return;
    }
    log::cout << log::endl;

    stopWatch.restart();
    Maker(opt).generate();
}

}

