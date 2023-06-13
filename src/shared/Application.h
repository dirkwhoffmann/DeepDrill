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

#pragma once

#include "config.h"
#include "Options.h"
#include "Logger.h"

namespace dd {

class Application {

protected:

    // Asset manager
    AssetManager assets;

    // Config options
    Options opt = Options(assets);

    // Stop watch for measuring total execution time
    Clock stopWatch;

public:

    // Main entry point
    int main(int argc, char *argv[]);

    // Returns a version string
    static string version();

private:

    // Sets up the GMP library
    void setupGmp();

    // Performs some common command line arguments checks
    void checkSharedArguments();

    // Reads all input files
    void readInputs();

    // Reads all profiles
    void readProfiles();


    //
    // Methods provided by subclasses
    //

    // Returns the app name
    virtual const char *appName() = 0;

    // Prints the command line syntax
    virtual void syntax() = 0;

    // Performs some basic initialization on program launch
    virtual void initialize() = 0;

    // Parses all command line arguments
    virtual void parseArguments(int argc, char *argv[]) = 0;

    // Checks all command line arguments for consistency
    virtual void checkCustomArguments() = 0;

    // Main method
    virtual void run() = 0;
};

}
