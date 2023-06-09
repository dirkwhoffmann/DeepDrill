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

#include "Application.h"
#include "Exception.h"
#include "Logger.h"
#include "Parser.h"

namespace dd {

int
Application::main(int argc, char **argv)
{
    try {

        log::cout << appName() << " " << Options::version();
        log::cout << " - (C)opyright Dirk W. Hoffmann";
        log::cout << log::endl << log::endl;

        // Perform some app specific initialization
        initialize();
        
        // Parse command line arguments
        parseArguments(argc, argv);

        // Check arguments for consistency
        checkSharedArguments();
        checkCustomArguments();

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

        // Run the application
        run();

        log::cout << log::vspace << "Total time: " << stopWatch.stop() << log::endl;

    } catch (dd::SyntaxError &e) {

        syntax();
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

void
Application::setupGmp()
{
    isize accuracy = 64;

    auto path = assets.findAsset(opt.files.inputs.front());

    if (AssetManager::getFormat(path) == Format::LOC) {

        /* If a location is given, we need to adjust the GMP precision based
         * on the zoom factor. Because we haven't parsed any input file when
         * this function is called, we need to peek this value directly from
         * the location file.
         */
        Parser::parse(path, [&accuracy](string key, string value) {

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
Application::checkSharedArguments()
{
    // The user needs to specify a single input
    if (opt.files.inputs.size() < 1) throw SyntaxError("No input file is given");
    if (opt.files.inputs.size() > 1) throw SyntaxError("More than one input file is given");

    // All profiles must exist
    for (auto &it : opt.files.profiles) (void)assets.findAsset(it, Format::PRF);
}

void
Application::readInputs()
{
    for (auto &input: opt.files.inputs) {

        if (AssetManager::getFormat(input) == Format::MAP) continue;

        Parser::parse(assets.findAsset(input),
                      [this](string k, string v) { opt.parse(k,v); });
    }
}

void
Application::readProfiles()
{
    for (auto &profile: opt.files.profiles) {

        Parser::parse(assets.findAsset(profile),
                      [this](string k, string v) { opt.parse(k,v); });
    }
}

}
