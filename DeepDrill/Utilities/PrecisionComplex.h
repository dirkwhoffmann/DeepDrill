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
#include "gmpxx.h"

namespace dd {

struct PrecisionComplex {
    
    mpf_class re;
    mpf_class im;
    
    PrecisionComplex() : re(0), im(0) { }
    PrecisionComplex(const mpf_class &r, const mpf_class &i) : re(r), im(i) { }
    PrecisionComplex(const struct StandardComplex &c) : re(c.re), im(c.im) { }

    inline double norm() const {
        
        double r = re.get_d();
        double i = im.get_d();
        return r*r + i*i;
    }

    PrecisionComplex &operator=(const StandardComplex &other);

    inline PrecisionComplex &operator+=(const PrecisionComplex &other) {
    
        re += other.re;
        im += other.im;
        return *this;
    }

    inline PrecisionComplex &operator-=(const PrecisionComplex &other) {
    
        re -= other.re;
        im -= other.im;
        return *this;
    }

    inline PrecisionComplex &operator*=(const PrecisionComplex &other) {
    
        const mpf_class r = re *other.re - im * other.im;
        im = re * other.im + im * other.re;
        re = r;
        return *this;
    }

    inline PrecisionComplex &operator/=(double other) {
    
        re /= other;
        im /= other;
        return *this;
    }

    inline PrecisionComplex operator+(const PrecisionComplex &other) const {
        
        return PrecisionComplex(re + other.re, im + other.im);
    }
            
    inline PrecisionComplex operator-(const PrecisionComplex &other) const {
        
        return PrecisionComplex(re - other.re, im - other.im);
    }

    inline PrecisionComplex operator*(const PrecisionComplex &other) const {
        
        return PrecisionComplex(re *other.re - im * other.im, re * other.im + im * other.re);
    }
    
    //
    // Pretty printing
    //
    
    friend std::ostream& operator<<(std::ostream& os, const PrecisionComplex& value);
};

}
