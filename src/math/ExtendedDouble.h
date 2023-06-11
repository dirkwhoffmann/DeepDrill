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
#include "gmpxx.h"
#include <cmath>

namespace dd {

struct ExtendedDouble {
  
    double mantissa;
    long exponent;

    
    //
    // Initializing
    //
    
    ExtendedDouble() : mantissa(0), exponent(0) { }
    ExtendedDouble(double m, long e) : mantissa(m), exponent(e) { }
    ExtendedDouble(double m);
    ExtendedDouble(const mpf_class &value);
    
    
    //
    // Converting
    //
    
    inline double asDouble() const {
        
        return ldexp(mantissa, (int)exponent);
    }

    
    //
    // Normalizing
    //
    
    inline void reduce() {

        int exp;
        mantissa = frexp(mantissa, &exp);
        exponent += exp;
    }
    
    bool isReduced() const {

        auto pos = std::abs(mantissa);
        return mantissa == 0.0 ? exponent == 0 : pos >= 0.5 && pos < 1.0;
    }
    
        
    //
    // Assigning
    //
    
    ExtendedDouble &operator=(const struct ExtendedDouble &other)
    {
        mantissa = other.mantissa;
        exponent = other.exponent;
        return *this;
    }

    ExtendedDouble &operator=(const mpf_class &other);
    
    
    //
    // Comparing
    //
    
    inline bool operator==(const ExtendedDouble &other) const;
    
    
    //
    // Calculating
    //
    
    inline ExtendedDouble &operator+=(const ExtendedDouble &other) {
    
        if (exponent == other.exponent) {
            mantissa += other.mantissa;
        } else if (exponent > other.exponent) {
            mantissa += other.mantissa / pow(2, exponent - other.exponent);
        } else {
            mantissa /= pow(2, other.exponent - exponent);
            exponent = other.exponent;
            mantissa += other.mantissa;
        }
        return *this;
    }

    inline ExtendedDouble &operator-=(const ExtendedDouble &other) {
        
        if (exponent == other.exponent) {
            mantissa -= other.mantissa;
        } else if (exponent > other.exponent) {
            mantissa -= other.mantissa / pow(2, exponent - other.exponent);
        } else {
            mantissa /= pow(2, other.exponent - exponent);
            exponent = other.exponent;
            mantissa -= other.mantissa;
        }
        return *this;
    }
    
    inline ExtendedDouble &operator*=(const ExtendedDouble &other) {
        
        mantissa *= other.mantissa;
        exponent += other.exponent;
        return *this;
    }

    inline ExtendedDouble &operator*=(double scale) {
    
        mantissa *= scale;
        return *this;
    }

    inline ExtendedDouble &operator/=(const ExtendedDouble &other) {
        
        mantissa /= other.mantissa;
        exponent -= other.exponent;
        return *this;
    }

    inline ExtendedDouble operator+(const ExtendedDouble &other) const {
        
        ExtendedDouble result = *this;
        result += other;
        return result;
    }
            
    inline ExtendedDouble operator-(const ExtendedDouble &other) const {
        
        ExtendedDouble result = *this;
        result -= other;
        return result;
    }

    inline ExtendedDouble operator*(const ExtendedDouble &other) const {
        
        ExtendedDouble result = *this;
        result *= other;
        return result;
    }

    inline ExtendedDouble operator*(double scale) const {

        ExtendedDouble result = *this;
        result *= scale;
        return result;
    }

    inline ExtendedDouble operator/(const ExtendedDouble &other) const {
        
        ExtendedDouble result = *this;
        result /= other;
        return result;
    }

    inline ExtendedDouble reciprocal() const {

        ExtendedDouble result = ExtendedDouble { 1.0 / mantissa, -exponent };
        return result;
    }

    inline ExtendedDouble log2() const {

        ExtendedDouble result = ExtendedDouble(std::log2(mantissa) + exponent);
        return result;
    }

    
    //
    // Comparing
    //
    
    bool operator<(ExtendedDouble &other);
    bool operator>(ExtendedDouble &other);
    bool operator<(double other);
    bool operator>(double other);
 };

}
