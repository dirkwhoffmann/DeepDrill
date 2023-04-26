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
#include "Buffer.h"

namespace dd {

class Palette {

    Buffer <u32> r;
    Buffer <u32> g;
    Buffer <u32> b;

public:

    Palette();

    void load(const string &path);
    u32 interpolateABGR(double value);
};

}
