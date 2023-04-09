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

template <class T> struct Animated {

    T current = 0;
    T target = 0;
    T deltaStep = 0;

    Animated(T value = 0) {

        set(value);
    }

    bool animates() const {
        return current != target;
    }

    void set(T value) {

        current = value;
        target = value;
    }

    void set(T value, long steps) {

        target = value;
        deltaStep = std::abs(target - current) / steps;
    }

    void move() {
        
        if (current + deltaStep <= target) {
            current += deltaStep;
        } else if (current - deltaStep >= target) {
            current -= deltaStep;
        } else {
            current = target;
        }
    }
};

struct Animated2 {

    double current = 1.0;
    double target = 1.0;
    double factor = 1.0;

    Animated2(double value = 0) {

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
