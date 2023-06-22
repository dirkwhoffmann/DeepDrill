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

#include "DeepZoom.h"
#include "Zoomer.h"

int main(int argc, char *argv[])
{
    return dd::DeepZoom().main(argc, argv);
}

namespace dd {

const char *
DeepZoom::optstring() const
{
    return ":vba:o:";
}

const option *
DeepZoom::longopts() const
{
    static struct option long_options[] = {

        { "verbose",  no_argument,       NULL, 'v' },
        { "batch",    no_argument,       NULL, 'b' },
        { "assets",   required_argument, NULL, 'a' },
        { "output",   required_argument, NULL, 'o' },
        { NULL,       0,                 NULL,  0  }
    };

    return long_options;
}

void
DeepZoom::syntax() const
{
    log::cout << "Usage: ";
    log::cout << "deepzoom [-bv] [-o <output>] <input>" << log::endl;
    log::cout << log::endl;
    log::cout << "       -b or --batch     Run in batch mode" << log::endl;
    log::cout << "       -v or --verbose   Run in verbose mode" << log::endl;
    log::cout << "       -a or --assets    Optional path to asset files" << log::endl;
}

void
DeepZoom::initialize()
{
    // Check for shader support
    if (!sf::Shader::isAvailable()) {
        throw Exception("No GPU shader support");
    }

    // Initialize FFmpeg
    FFmpeg::init();
}

bool
DeepZoom::isAcceptedInputFormat(Format format) const
{
    return format == Format::DIR;
}

bool
DeepZoom::isAcceptedOutputFormat(Format format) const
{
    return AssetManager::isVideoFormat(format);
}

void
DeepZoom::checkArguments()
{
    auto iniFiles = opt.getInputs(Format::INI);

    // A single input file must be given
    if (opt.files.inputs.size() < 1) throw SyntaxError("No input file is given");
    if (opt.files.inputs.size() > 1) throw SyntaxError("More than one input file is given");

    // At most one output file must be given
    if (opt.files.outputs.size() > 1) throw SyntaxError("More than one output file is given");

    // The input must be an existing directory
    (void)assets.findAsset(opt.files.inputs.front(), Format::DIR);

    // The directory must contain an .ini file for the zoomer
    // TODO: Check for this file
    opt.files.inputs.push_back(opt.files.inputs.front() / "deepzoom.ini");

    if (!opt.files.outputs.empty()) {

        auto output = opt.files.outputs.front();

        // The output file must be writable
        std::ofstream file(output, std::ofstream::out);
        if (!file.is_open()) {
            throw SyntaxError("Can't write to file " + output.string());
        }

        // FFmpeg must be installed
        if (!FFmpeg::available()) {
            throw Exception("Unable to locate FFmpeg.");
        }
    }
}

void
DeepZoom::run()
{
    Zoomer(opt).launch();
}

}
