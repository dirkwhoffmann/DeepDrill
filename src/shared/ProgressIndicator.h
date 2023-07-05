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
#include "Chrono.h"
#include "Logger.h"

namespace dd {

class ProgressIndicator {

    // Description
    string description;
    
    // Progress
    isize progress;
    isize progressMax;
    
    // Printed dots
    isize dots;
    isize dotsMax;
    
    // Stop watch
    Clock clock;
        

    //
    // Methods
    //
    
public:

    ProgressIndicator() { };
    ProgressIndicator(const string &description, isize max = 100);
    ~ProgressIndicator();

    void step(isize delta = 1);
    void done(const string &info = "");

// private:
    
    void init(const string &description, isize max = 100);
};

class BatchProgressIndicator {

    string msg;
    fs::path path;
    Clock clock;

public:

    BatchProgressIndicator(const string &msg, const fs::path &path);
    ~BatchProgressIndicator();

private:

    void prefix(Logger &logger);
};

}
