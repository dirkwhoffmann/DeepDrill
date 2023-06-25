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

int main(int argc, char *argv[])
{
    return dd::DeepMake().main(argc, argv);
}

namespace dd {

const char *
DeepMake::optstring() const
{
    return ":va:o:";
}

const option *
DeepMake::longopts() const
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
DeepMake::syntax() const
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
    // Collect paths to all files that need to be created, skipped or modified
    std::vector <fs::path> create;
    std::vector <fs::path> skipOrModify;
    auto add = [&](const fs::path &path) {
        fs::exists(path) ? skipOrModify.push_back(path) : create.push_back(path);
    };
    auto project = opt.files.outputs.front();
    add(project / "Makefile");
    add(project / AssetManager::iniFile());
    for (isize i = 0; i < opt.video.keyframes; i++) {
        add(project / AssetManager::iniFile(i));
    }

    log::cout << log::vspace;
    log::cout << log::ralign(std::to_string(create.size()), 5);
    log::cout << " files will be created. " << log::endl;
    log::cout << log::ralign(std::to_string(skipOrModify.size()), 5);
    log::cout << " files will be skipped or modified." << log::endl;
    log::cout << log::endl;

    while (1) {

        log::cout << "Do you want to proceed [y]? ";

        string s; std::getline(std::cin, s);
        stopWatch.restart();

        if (s == "y" || s == "yes" || s == "") {

            Maker(*this, opt).generate();
            return;
        }
        if (s == "n" || s == "no") {

            return;
        }

        std::cout << '\a';
    }
}

}

