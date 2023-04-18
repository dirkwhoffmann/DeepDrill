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
#include "Types.h"
#include "commons.h"

namespace dd {

class FFmpeg {

public:

    // A list of available FFmpeg executables (setup in init() )
    static std::vector<string> paths;

    // Path to the selected FFmpeg executable
    static string exec;

    FILE *handle = nullptr;


    //
    // Locating FFmpeg
    //

    // Sets up the 'path' vector
    static void init();

    // Getter and setter for the FFmpeg executable path
    static const string getExecPath();
    static void setExecPath(const string &path);

    // Checks whether FFmeg is available
    static bool available();


    //
    // Running FFmpeg
    //

    // Launches the FFmpeg instance
    bool launch(const string &args);

    // Returns true if the FFmpeg instance is currently running
    bool isRunning();

    // Waits until the FFmpeg instance has terminated
    void join();
};

}
