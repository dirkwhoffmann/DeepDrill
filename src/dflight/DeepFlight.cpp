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

#include "DeepFlight.h"
#include "Exception.h"
#include "Logger.h"
#include "Parser.h"
#include "Zoomer.h"

#include <getopt.h>

int main(int argc, char *argv[])
{
    using namespace dd;

    try {

        DeepFlight().main(argc, argv);

    } catch (dd::SyntaxError &e) {

        log::cout << "Usage: ";
        log::cout << "deepflight [-bv] [-p <profile>] -o <output> <input>" << log::endl;
        log::cout << log::endl;
        log::cout << "       -b or --batch     Run in batch mode" << log::endl;
        log::cout << "       -v or --verbose   Run in verbose mode" << log::endl;
        log::cout << "       -a or --assets    Optional path to asset files" << log::endl;
        log::cout << "       -p or --profile   Customize settings" << log::endl;
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
DeepFlight::main(int argc, char *argv[])
{
    log::cout << "DeepFlight " << Options::version();
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
    Zoomer(opt).launch();

    log::cout << log::vspace << "Total time: " << stopWatch.stop() << log::endl;
}

void
DeepFlight::parseArguments(int argc, char *argv[])
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

    // Remember the path to the executable
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
        inputs.push_back(argv[optind++]);
    }
}

void
DeepFlight::checkArguments()
{
    // The user needs to specify a single input
    if (inputs.size() < 1) throw SyntaxError("No input file is given");
    if (inputs.size() > 1) throw SyntaxError("More than one input file is given");
    auto input = inputs.front();

    // The user needs to specify at most one output
    if (outputs.size() > 1) throw SyntaxError("More than one output file is given");

    // All profiles must exist
    for (auto &it : profiles) (void)assets.findAsset(it, Format::PRF);

    // The input files must be a prj file
    opt.files.input = assets.findAsset(input, Format::PRJ);

    if (!outputs.empty()) {

        auto output = opt.files.output = outputs.front();

        // The output file must be a video file
        AssetManager::assureFormat(opt.files.output, Format::MPG);

        // The output file must be writable
        std::ofstream file(opt.files.output, std::ofstream::out);
        if (!file.is_open()) {
            throw SyntaxError("Can't write to file " + opt.files.output.string());
        }
    }
}

void
DeepFlight::readInputs()
{
    Parser::parse(opt.files.input,
                  [this](string k, string v) { opt.parse(k,v); });
}

void
DeepFlight::readProfiles()
{
    for (auto &profile: profiles) {

        Parser::parse(assets.findAsset(profile, Format::PRF),
                      [this](string k, string v) { opt.parse(k,v); });
    }
}

void
DeepFlight::setupGmp()
{
    mpf_set_default_prec(128);
}

}
