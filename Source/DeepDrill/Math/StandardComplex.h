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
#include <StandardComplex.h>
#include <ostream>

namespace dd {

struct StandardComplex {
  
    double re;
    double im;

    
    //
    // Initializing
    //
    
    StandardComplex() : re(0), im(0) { }
    StandardComplex(const double r, const double i) : re(r), im(i) { }
    StandardComplex(const struct PrecisionComplex &other);
    
    
    //
    // Converting
    //
    
    inline double norm() const { return re * re + im * im; }
    
    
    //
    // Assigning
    //
    
    StandardComplex &operator=(const struct PrecisionComplex &other);

    
    //
    // Comparing
    //
    
    inline bool operator==(const StandardComplex &other) const {
        
        return re == other.re && im == other.im;
    }
    
        
    //
    // Calculation
    //
    
    inline StandardComplex &operator+=(const StandardComplex &other) {
    
        re += other.re;
        im += other.im;
        return *this;
    }

    inline StandardComplex &operator-=(const StandardComplex &other) {
    
        re -= other.re;
        im -= other.im;
        return *this;
    }

    inline StandardComplex &operator*=(const StandardComplex &other) {
    
        const double r = re *other.re - im * other.im;
        im = re * other.im + im * other.re;
        re = r;
        return *this;
    }

    inline StandardComplex &operator*=(double other) {
    
        re *= other;
        im *= other;
        return *this;
    }
    
    inline StandardComplex &operator/=(const StandardComplex &other) {
    
        *this *= other.reciprocal();
        return *this;
    }
    
    inline StandardComplex operator+(const StandardComplex &other) const {
        
        return StandardComplex(re + other.re, im + other.im);
    }
            
    inline StandardComplex operator-(const StandardComplex &other) const {
        
        return StandardComplex(re - other.re, im - other.im);
    }

    inline StandardComplex operator*(const StandardComplex &other) const {
        
        return StandardComplex { re *other.re - im * other.im, re * other.im + im * other.re };
    }

    inline StandardComplex operator*(double other) const {
        
        return StandardComplex { re * other, im * other };
    }
    
    inline StandardComplex operator/(const StandardComplex &other) const {
        
        StandardComplex result = *this;
        result /= other;
        return result;
    }

    inline StandardComplex square() const {

        return StandardComplex { re * re - im * im, re * im + im * re };
    }
    
    inline StandardComplex conjugate() const {
        
        return StandardComplex { re, -im };
    }

    inline StandardComplex reciprocal() const {
        
        auto nrm = norm();
        return StandardComplex { re / nrm, -im / nrm };
    }

    
    //
    // Pretty printing
    //
    
    friend std::ostream& operator<<(std::ostream& os, const StandardComplex& value);
};

}
