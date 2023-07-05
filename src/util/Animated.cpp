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

#include "Animated.h"
#include <tgmath.h>

void
Animated::set(double value)
{
    current = value;
}

void
Animated::move(double velocity)
{
    current *= pow(2.0, velocity / scale);
}
