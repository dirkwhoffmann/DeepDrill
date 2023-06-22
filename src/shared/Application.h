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

#include <getopt.h>

namespace dd {

class Application {

protected:

    // Asset manager
    AssetManager assets;

    // Config options
    Options opt = Options(assets);

    // Stop watch for measuring the total execution time
    Clock stopWatch;

public:
    
    // Main entry point
    int main(int argc, char *argv[]);

    // Returns a version string
    static string version();
    static string version(isize major, isize minor, isize subminor, isize beta);

    // Reads all ini files
    void readIniFiles(isize keyframe = 0);

private:

    // Configures the application (called in main)
    void configure();
    
    // Sets up the GMP library
    void setupGmp();

protected:

    // Parses all command line arguments
    void parseArguments(int argc, char *argv[], const char *optstr, const option *longopts);


    //
    // Methods provided by subclasses
    //

private:
    
    // Returns the app name
    virtual const char *appName() = 0;

    // Returns the getopt command line parameters
    virtual const char *optstring() = 0;
    virtual const struct option *longopts() = 0;

    // Prints the command line syntax
    virtual void syntax() = 0;

    // Performs some basic initialization on program launch
    virtual void initialize() = 0;

    // Checks the input and output file formats for validity
    virtual bool isAcceptedInputFormat(Format format) const = 0;
    virtual bool isAcceptedOutputFormat(Format format) const = 0;

    // Checks all command line arguments for consistency
    virtual void checkArguments() = 0;

    // Main method
    virtual void run() = 0;
};

}
