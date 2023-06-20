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

// #include <getopt.h>

namespace dd {

int
Application::main(int argc, char *argv[])
{
    mpf_set_default_prec(128);

    try {

        log::cout << appName() << " " << version();
        log::cout << " - (C)opyright Dirk W. Hoffmann";
        log::cout << log::endl << log::endl;

        // Perform some app specific initialization
        initialize();
        
        // Parse command line arguments
        parseArguments(argc, argv, optstring(), longopts());

        // Check arguments for consistency
        checkArguments();

        // Customize settings
        readIniFiles();

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

void
Application::parseArguments(int argc, char *argv[], const char *optstr, const option *longopts)
{
    // Don't print the default error messages
    opterr = 0;

    // Remember the path to the DeppDrill executable
    opt.files.exec = makeAbsolutePath(argv[0]);

    // Parse all options
    while (1) {

        int arg = getopt_long(argc, argv, optstr, longopts, NULL);
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

    // Check types
    for (const auto &it : opt.files.inputs) {
        if (!isAcceptedInputFormat(AssetManager::getFormat(it))) {
            throw Exception(it.string() + ": Invalid input format");
        }
    }
    for (const auto &it : opt.files.outputs) {
        if (!isAcceptedOutputFormat(AssetManager::getFormat(it))) {
            throw Exception(it.string() + ": Invalid output format");
        }
    }
}

void
Application::readIniFiles(isize keyframe)
{
    auto iniFiles = opt.getInputs(Format::INI);

    for (auto &it: iniFiles) {

        Parser::parse(assets.findAsset(it),
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
