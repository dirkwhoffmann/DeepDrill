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
#include "DeepTravel.h"
#include "Logger.h"
#include <getopt.h>

int main(int argc, char *argv[])
{
    using namespace dd;

    try {

        DeepTravel().main(argc, argv);

    } catch (dd::SyntaxError &e) {

        log::cout << "Usage: ";
        log::cout << "deeptravel [-bmv] [-p <profile>] -o <output> <input>" << log::endl;
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
DeepTravel::main(int argc, char *argv[])
{
    map<string,string> keys;
    string option = "";

    // Parse all command line arguments
    // parseArguments(argc, argv, keys);

    log::cout << "DeepTravel " << VER_MAJOR << "." << VER_MINOR;
    log::cout << " - (C)opyright Dirk W. Hoffmann";
    log::cout << log::endl << log::endl;

    // Setup the GMP library
    setupGmp(keys);

    /*
    // Parse all options
    Options opt(keys);

    // Start a stop watch
    Clock stopWatch;

    // Execute
    opt.make ? runMaker(opt) : runPipeline(opt);

    log::cout << log::vspace << "Total time: " << stopWatch.stop() << log::endl;
    */
}

void
DeepTravel::setupGmp(std::map <string,string> &keys)
{

}

}
