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

struct Animated {

    double current = 1.0;
    double target = 1.0;
    double factor = 1.0;

    Animated(double value = 0);

    bool animates() const;
    void set(double value);
    void set(double value, long steps);
    void move();

    void setFactor(long steps);
};
