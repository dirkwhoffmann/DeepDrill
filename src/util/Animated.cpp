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

Animated::Animated(double value)
{
    set(value);
}

bool Animated::animates() const
{
    return current != target;
}

void Animated::set(double value)
{
    current = value;
    target = value;
}

void Animated::set(double value, long steps)
{
    assert(current != 0);

    target = value;
    factor = pow(target / current, 1.0 / steps);
}

void Animated::move()
{
    if (current > target && current * factor >= target) {
        current *= factor;
    } else if (current < target && current * factor <= target) {
        current *= factor;
    } else {
        current = target;
    }
}
