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

struct Animated {

    double current = 1.0;
    double target = 1.0;
    double factor = 1.0;

    Animated(double value = 0) {

        set(value);
    }

    bool animates() const {
        return current != target;
    }

    void set(double value) {

        current = value;
        target = value;
    }

    void set(double value, long steps) {

        assert(current != 0);

        target = value;
        factor = pow(target / current, 1.0 / steps);
    }

    void move() {

        if (current > target && current * factor >= target) {
            current *= factor;
        } else if (current < target && current * factor <= target) {
            current *= factor;
        } else {
            current = target;
        }
    }
};
