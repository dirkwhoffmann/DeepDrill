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
#include "SlowDriller.h"
#include "Driller.h"
#include "DrillMap.h"
#include "MapAnalyzer.h"
#include "ProgressIndicator.h"
#include "Compressor.h"

int main(int argc, char *argv[])
{
    return dd::DeepDrill().main(argc, argv);
}

namespace dd {

const char *
DeepDrill::optstring() const
{
    return ":vba:o:";
}

const option *
DeepDrill::longopts() const
{
    static struct option long_options[] = {

        { "batch",    no_argument,       NULL, 'b' },
        { "verbose",  no_argument,       NULL, 'v' },
        { "assets",   required_argument, NULL, 'a' },
        { "output",   required_argument, NULL, 'o' },
        { NULL,       0,                 NULL,  0  }
    };

    return long_options;
}

void
DeepDrill::syntax() const
{
    log::cout << "Usage: ";
    log::cout << "deepdrill [-bv] [-a <path>] -o <output> [<keyvalue>] <inputs>" << log::endl;
    log::cout << log::endl;
    log::cout << "       -b or --batch     Run in batch mode" << log::endl;
    log::cout << "       -v or --verbose   Run in verbose mode" << log::endl;
    log::cout << "       -a or --assets    Optional path to asset files" << log::endl;
    log::cout << "       -o or --output    Output file" << log::endl;
}

bool
DeepDrill::isAcceptedInputFormat(Format format) const
{
    return format == Format::MAP || format == Format::INI;
}

bool
DeepDrill::isAcceptedOutputFormat(Format format) const
{
    return format == Format::MAP || AssetManager::isImageFormat(format);
}

void
DeepDrill::checkArguments()
{
    // At most one map file must be given
    if (Options::getInputs(Format::MAP).size() > 1) throw SyntaxError("More than one map file is given");

    // At least one output file must be given
    if (Options::files.outputs.size() < 1) throw SyntaxError("No output file is given");
}

void
DeepDrill::run()
{
    // Initialize the drill map
    drillMap.resize();

    if (!Options::getInputs(Format::MAP).empty()) {

        // Load the drill map from disk
        drillMap.load(Options::getInputs(Format::MAP).front());

        // Generate outputs
        generateOutputs();

    } else {

        BatchProgressIndicator progress("Drilling",  Options::files.outputs.front());

        // Run the driller
        runDriller();

        // Generate outputs
        generateOutputs();

        // Analyze the drill map
        if (Options::flags.verbose) MapAnalyzer(drillMap).print();
    }
}

void
DeepDrill::runDriller()
{
    if (Options::perturbation.enable) {

        Driller driller(drillMap);
        driller.drill();

    } else {

        SlowDriller driller(drillMap);
        driller.drill();
    }
}

void
DeepDrill::generateOutputs()
{
    for (auto &it : Options::files.outputs) {

        auto outputFormat = AssetManager::getFormat(it);

        if (AssetManager::isImageFormat(outputFormat)) {

            // Create and save image file
            imageMaker.draw(drillMap);
            imageMaker.save(it, outputFormat);

        } else {

            // Save map file
            drillMap.save(it);
        }
    }
}

}
