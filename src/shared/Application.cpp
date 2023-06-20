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
    mpf_set_default_prec(128);

    try {

        log::cout << appName() << " " << version();
        log::cout << " - (C)opyright Dirk W. Hoffmann";
        log::cout << log::endl << log::endl;

        // Perform some app specific initialization
        initialize();
        
        // Parse command line arguments
        parseArguments(argc, argv);

        // Check arguments for consistency
        checkArguments();

        // Customize settings
        readIniFiles();
        readProfiles(); // REMOVE

        // Setup the GMP library
        setupGmp();

        // Deduce missing options
        opt.derive();

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

    // Derive GMP precision from the zoom factor
    long exponent = 0;
    mpf_get_d_2exp(&exponent, opt.location.zoom.get_mpf_t());
    accuracy = exponent + 64;

    // Set the new precision
    mpf_set_default_prec(accuracy);

    // Resize the GMP location variable
    opt.location.real.set_prec(accuracy);
    opt.location.imag.set_prec(accuracy);

    // Parse the location again to make use of the new precision
    opt.parse("location.real", opt.keys["location.real"]);
    opt.parse("location.imag", opt.keys["location.imag"]);
}

/*
void
Application::setupGmp()
{
    isize accuracy = 64;

    // To initialize GMP appropriately, we need to know the zoom factor.
    // Because we haven't parsed any input file when this function is called,
    // we need to peek this value directly from the ini files.

    printf("Precision of re: %ld\n", opt.location.real.get_prec());

    for (auto &it : opt.files.inifiles) {

        auto path = assets.findAsset(it);

        Parser::parse(path, [&accuracy, this](string key, string value) {

            if (key == "location.zoom") {

                printf("Found zoom key\n");

                try {

                    long exponent = 0;
                    auto zoom = mpf_class(value);
                    mpf_get_d_2exp(&exponent, zoom.get_mpf_t());
                    accuracy = exponent + 64;
                    printf("accuracy = %ld\n", accuracy);

                    printf("New precision of re: %ld\n", opt.location.real.get_prec());
                    return;

                } catch (...) { }
            }
        });
    }

    mpf_set_default_prec(accuracy);
}
*/

void
Application::readInputs(isize keyframe)
{
    for (auto &input: opt.files.inputs) {

        if (AssetManager::getFormat(input) == Format::MAP) continue;

        Parser::parse(assets.findAsset(input),
                      [this](string k, string v) { opt.parse(k, v); },
                      keyframe);
    }
}

void
Application::readIniFiles(isize keyframe)
{
    for (auto &inifile: opt.files.inifiles) {

        Parser::parse(assets.findAsset(inifile),
                      [this](string k, string v) { opt.parse(k, v); },
                      keyframe);
    }
}

void
Application::readProfiles(isize keyframe)
{
    for (auto &profile: opt.files.profiles) {

        Parser::parse(assets.findAsset(profile),
                      [this](string k, string v) { opt.parse(k, v); },
                      keyframe);
    }
}

string
Application::version()
{
    auto result = version(VER_MAJOR, VER_MINOR, VER_SUBMINOR, VER_BETA);
    if (!releaseBuild) result += " [DEBUG BUILD]";

    return result;
}

string
Application::version(isize major, isize minor, isize subminor, isize beta)
{
    auto result = std::to_string(major) + "." + std::to_string(minor);

    if (subminor) result += "." + std::to_string(subminor);
    if (beta) result += "b" + std::to_string(beta);

    return result;
}

}
