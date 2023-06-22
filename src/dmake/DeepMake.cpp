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

// #include <getopt.h>

int main(int argc, char *argv[])
{
    return dd::DeepMake().main(argc, argv);
}

namespace dd {

const char *
DeepMake::optstring()
{
    return ":va:o:";
}

const option *
DeepMake::longopts()
{
    static struct option long_options[] = {

        { "verbose",  no_argument,       NULL, 'v' },
        { "assets",   required_argument, NULL, 'a' },
        { "output",   required_argument, NULL, 'o' },
        { NULL,       0,                 NULL,  0  }
    };

    return long_options;
}

void
DeepMake::syntax()
{
    log::cout << "Usage: ";
    log::cout << "deepmake [-v] [-a <path>] -o <output> <inputs>" << log::endl;
    log::cout << log::endl;
    log::cout << "       -v or --verbose   Run in verbose mode" << log::endl;
    log::cout << "       -a or --assets    Optional path to asset files" << log::endl;
    log::cout << "       -o or --output    Output file" << log::endl;
}

void
DeepMake::checkArguments()
{
    // A single output file must be given
    if (opt.files.outputs.size() < 1) throw SyntaxError("No output file is given");
    if (opt.files.outputs.size() > 1) throw SyntaxError("More than one output file is given");

    // The output must be an existing directory
    (void)assets.findAsset(opt.files.outputs.front(), Format::DIR);
}

void
DeepMake::run()
{
    // Determine the number of computed files
    auto numFiles = opt.video.keyframes + 3;

    auto askForPermission = [&numFiles]() {

        while (1) {

            log::cout << numFiles << " files will be created. Do you want to proceed [y]? ";

            string s; std::getline(std::cin, s);

            if (s == "y" || s == "yes" || s == "") return true;
            if (s == "n" || s == "no") return false;

            std::cout << '\a';
        }
    };

    auto permission = askForPermission();
    stopWatch.restart();
    if (permission) Maker(*this, opt).generate();
}

}

