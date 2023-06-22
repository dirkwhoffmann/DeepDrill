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
#include "Application.h"

namespace dd {

class DeepZoom : public Application {

    //
    // Methods from Application
    //

    const char *appName() const { return "DeepZoom"; }
    const char *optstring() const;
    const option *longopts() const;
    bool isAcceptedInputFormat(Format format) const;
    bool isAcceptedOutputFormat(Format format) const;

    void syntax();
    void initialize();
    void checkArguments();
    void run();
};

}
