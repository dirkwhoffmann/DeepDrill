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
#include "StandardComplex.h"
#include "ExtendedDouble.h"
#include "PrecisionComplex.h"
#include <tgmath.h>

namespace dd {

struct ExtendedComplex {
  
    StandardComplex mantissa;
    long exponent;

    
    //
    // Initializing
    //
    
    ExtendedComplex() : exponent(0) { }
    ExtendedComplex(const double &re, const double &im);
    ExtendedComplex(const ExtendedDouble &re, const ExtendedDouble &im);
    ExtendedComplex(const mpf_class &re, const mpf_class &im);
    ExtendedComplex(const StandardComplex &m, long e) : mantissa(m), exponent(e) { }
    ExtendedComplex(const StandardComplex &m) : mantissa(m), exponent(0) { };
    ExtendedComplex(const PrecisionComplex &other) : ExtendedComplex(other.re, other.im) { };

    
    //
    // Converting
    //

    inline ExtendedDouble norm() const {
        
        return ExtendedDouble { mantissa.norm(), 2 * exponent };
    }

    inline ExtendedDouble abs() const {

        return ExtendedDouble { mantissa.abs(), exponent };
    }

    
    //
    // Normalizing
    //
    
    void reduce() {
        
        auto posre = std::abs(mantissa.re);
        auto posim = std::abs(mantissa.im);
        
        if (posre == 0 && posim == 0) {
            
            exponent = 0;
            
        } else if (posre > posim) {
            
            int exp;
            mantissa.re = frexp(mantissa.re, &exp);
            mantissa.im = ldexp(mantissa.im, -exp);
            exponent += exp;
            
        } else {
            
            int exp;
            mantissa.im = frexp(mantissa.im, &exp);
            mantissa.re = ldexp(mantissa.re, -exp);
            exponent += exp;
        }
    }
    
    bool isReduced() const {
    
        auto posre = std::abs(mantissa.re);
        auto posim = std::abs(mantissa.im);
        
        if (mantissa == StandardComplex {0,0}) {
            return exponent == 0;
        } else {
            return (posre >= 0.5 && posre < 1.0) || (posim >= 0.5 && posim < 1.0);
        }
    }
    
    void normalize() {

        *this *= abs().reciprocal();
        reduce();
    }


    //
    // Comparing
    //
    
    inline bool operator==(const ExtendedComplex &other) const {
        
        return mantissa == other.mantissa && exponent == other.exponent;
    }

    
    //
    // Calculating
    //
    
    inline ExtendedComplex &operator+=(const ExtendedComplex &other) {
        
        if (exponent > other.exponent) {
            mantissa += other.mantissa * ldexp(1.0, (int)(other.exponent - exponent));
        } else {
            mantissa *= ldexp(1.0, int(exponent - other.exponent));
            mantissa += other.mantissa;
            exponent = other.exponent;
        }
        return *this;
    }

    inline ExtendedComplex &operator-=(const ExtendedComplex &other) {
    
        if (exponent > other.exponent) {
            mantissa -= other.mantissa * ldexp(1.0, (int)(other.exponent - exponent));
        } else {
            mantissa *= ldexp(1.0, int(exponent - other.exponent));
            mantissa -= other.mantissa;
            exponent = other.exponent;
        }
        return *this;
    }

    inline ExtendedComplex &operator*=(const ExtendedComplex &other) {
    
        mantissa *= other.mantissa;
        exponent += other.exponent;
        return *this;
    }

    inline ExtendedComplex &operator*=(ExtendedDouble other) {
    
        mantissa.re *= other.mantissa;
        mantissa.im *= other.mantissa;
        exponent += other.exponent;
        return *this;
    }

    inline ExtendedComplex &operator*=(double other) {
    
        mantissa *= other;
        return *this;
    }

    inline ExtendedComplex &operator/=(const ExtendedComplex &other) {
    
        *this *= other.reciprocal();
        return *this;
    }

    inline ExtendedComplex operator+(const ExtendedComplex &other) const {
        
        ExtendedComplex result = *this;
        result += other;
        return result;
    }
            
    inline ExtendedComplex operator-(const ExtendedComplex &other) const {
        
        ExtendedComplex result = *this;
        result -= other;
        return result;
    }

    inline ExtendedComplex operator*(const ExtendedComplex &other) const {

        ExtendedComplex result = *this;
        result *= other;
        return result;
    }

    inline ExtendedComplex operator*(double scale) const {
        
        ExtendedComplex result = *this;
        result *= scale;
        return result;
    }

    inline ExtendedComplex operator/(const ExtendedComplex &other) const {
        
        ExtendedComplex result = *this;
        result /= other;
        return result;
    }

    inline ExtendedComplex square() const {

        ExtendedComplex result = *this;
        result *= *this;
        return result;
    }

    inline ExtendedComplex conjugate() const {
        
        ExtendedComplex result = *this;
        result.mantissa.im = -result.mantissa.im;
        return result;
    }

    inline ExtendedComplex reciprocal() const {
        
        ExtendedComplex result = *this;
        result.mantissa.im = -result.mantissa.im;
        result *= norm().reciprocal();
        return result;
    }    
};

}
