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
#include "StandardComplex.h"

namespace dd {

struct PrecisionComplex {
    
    mpf_class re;
    mpf_class im;


    //
    // Initializing
    //

    PrecisionComplex() : re(0), im(0) { }
    PrecisionComplex(double re, double im) : re(re), im(im) { }
    PrecisionComplex(const mpf_class &re, const mpf_class &im) : re(re), im(im) { }
    PrecisionComplex(const struct StandardComplex &c) : re(c.re), im(c.im) { }


    //
    // Converting
    //

    inline mpf_class norm() const {
        
        return re * re + im * im;
    }

    inline mpf_class abs() const {

        return sqrt(re * re + im * im);
    }


    //
    // Normalizing
    //

    inline void normalize() {

        mpf_class len = abs();

        if (len != 0) {

            re /= len;
            im /= len;
        }
    }


    //
    // Assigning
    //

    PrecisionComplex &operator=(const StandardComplex &other) {

        re = other.re;
        im = other.im;
        return *this;
    }


    //
    // Calculating
    //

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

    inline PrecisionComplex &operator*=(mpf_class other) {

        re *= other;
        im *= other;
        return *this;
    }

    inline PrecisionComplex &operator*=(double other) {

        re *= other;
        im *= other;
        return *this;
    }

    inline PrecisionComplex &operator/=(const PrecisionComplex &other) {

        mpf_class norm = re * re + im * im;
        mpf_class inv = 1.0 / norm;

        im = -im;
        im *= inv;
        re *= inv;
        return *this;
    }

    inline PrecisionComplex &operator/=(const mpf_class &other) {

        re /= other;
        im /= other;
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

    inline PrecisionComplex operator*(const mpf_class &other) const {

        return PrecisionComplex(re * other, im * other);
    }

    inline PrecisionComplex operator/(const PrecisionComplex &other) const {

        return *this * other.reciprocal();
    }

    inline PrecisionComplex operator/(const mpf_class &other) const {

        return PrecisionComplex(re / other, im / other);
    }

    inline PrecisionComplex reciprocal() const {

        PrecisionComplex result = *this;
        mpf_class norm = re * re + im * im;
        mpf_class inv = 1.0 / norm;

        result.im = -result.im;
        result *= inv;

        return result;
    }

};

}
