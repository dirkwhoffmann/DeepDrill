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

#include <cmath>
#include "gmpxx.h"

namespace dd {

struct ExtendedDouble {
  
    double mantissa;
    long exponent;

    
    //
    // Initializing
    //
    
    constexpr ExtendedDouble() : mantissa(0), exponent(0) { }
    constexpr ExtendedDouble(const double m, const long e) : mantissa(m), exponent(e) { }
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
    
    inline bool operator==(const ExtendedDouble &other) const {
        
        assert(isReduced());
        return mantissa == other.mantissa && exponent == other.exponent;
    }
    
    
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
        reduce();
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
        reduce();
        return *this;
    }
    
    inline ExtendedDouble &operator*=(const ExtendedDouble &other) {
        
        mantissa *= other.mantissa;
        exponent += other.exponent;
        reduce();
        return *this;
    }

    inline ExtendedDouble &operator*=(double scale) {
    
        mantissa *= scale;
        reduce();
        return *this;
    }

    inline ExtendedDouble &operator/=(const ExtendedDouble &other) {
        
        mantissa /= other.mantissa;
        exponent -= other.exponent;
        reduce();
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
        result.reduce();
        return result;
    }

    
    //
    // Comparing
    //
    
    inline bool operator<(ExtendedDouble &other) {

        reduce();
        other.reduce();

        if (mantissa == 0.0 && other.mantissa < 0.0) {
            return false;
         } if (mantissa == 0.0 && other.mantissa > 0.0) {
             return true;
         } else if (other.mantissa == 0.0 && mantissa < 0.0) {
             return true;
         } else if (other.mantissa == 0.0 && mantissa > 0.0) {
             return false;
         } else if (exponent < other.exponent) {
             return true;
         } else if (exponent > other.exponent) {
             return false;
         } else if (mantissa == other.mantissa) {
             return false;
         } else if (mantissa < other.mantissa) {
             return true;
         } else {
             return false;
         }
    }
    
    inline bool operator>(ExtendedDouble &other) {

        return other < *this;
    }

    inline bool operator<(double other) {

        ExtendedDouble tmp(other);
        return *this < tmp;
    }

    inline bool operator>(double other) {
        
        ExtendedDouble tmp(other);
        return *this > tmp;
    }

    
    //
    // Pretty printing
    //
    
    friend std::ostream& operator<<(std::ostream& os, const ExtendedDouble& value);
};

}
