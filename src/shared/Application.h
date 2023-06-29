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
#include "Chrono.h"
#include "Logger.h"

#include <getopt.h>
#include <thread>

namespace dd {

class Application {

protected:

    // Asset manager
    AssetManager assets;

    // Configuration options
    Options opt = Options(assets);

    // Stop watch for measuring the total execution time
    Clock stopWatch;

    // ID of the main thread
    static const std::thread::id MAIN_THREAD_ID;

public:
    
    // Main entry point
    int main(int argc, char *argv[]);

    // Returns a version string
    static string version();
    static string version(isize major, isize minor, isize subminor, isize beta);

    // Returns true if the current thread is the main thread
    static bool isMainThread() { return std::this_thread::get_id() == MAIN_THREAD_ID; }

    // Reads all ini files
    void readConfigFiles(isize keyframe = 0);

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
    virtual const char *appName() const = 0;

    // Returns the argument strings required by getopt
    virtual const char *optstring() const = 0;
    virtual const struct option *longopts() const = 0;

    // Prints the command line syntax
    virtual void syntax() const = 0;

    // Initializes or finalizes the application
    virtual void initialize() = 0;

    // Checks the input and output file formats for validity
    virtual bool isAcceptedInputFormat(Format format) const = 0;
    virtual bool isAcceptedOutputFormat(Format format) const = 0;

    // Checks all command line arguments for consistency
    virtual void checkArguments() = 0;

    // Runs the application (main method)
    virtual void run() = 0;
};

}
