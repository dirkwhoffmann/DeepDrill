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

#include "config.h"
#include "Types.h"
#include "ExtendedComplex.h"
#include <iostream>

namespace dd {

ExtendedComplex::ExtendedComplex(const StandardComplex &m)
{
    mantissa = m;
    exponent = 0;
}

ExtendedComplex::ExtendedComplex(const ExtendedDouble &re, const ExtendedDouble &im)
{
    long exp1 = re.exponent, exp2 = im.exponent;

    double mantissa1 = re.mantissa;
    double mantissa2 = im.mantissa;

    if (re.mantissa == 0) {

        mantissa = StandardComplex { re.mantissa, im.mantissa };
        exponent = im.exponent;

    } else if (im.mantissa == 0) {

        mantissa = StandardComplex { re.mantissa, im.mantissa };
        exponent = re.exponent;
        
    } else if (exp1 < exp2) {

        auto exp = (int)(exp1 - exp2);
        mantissa = StandardComplex { ldexp(mantissa1, exp), mantissa2 };
        exponent = exp2;

    } else {

        auto exp = (int)(exp2 - exp1);
        mantissa = StandardComplex { mantissa1, ldexp(mantissa2, exp) };
        exponent = exp1;
    }
}

ExtendedComplex::ExtendedComplex(const double &re, const double &im)
{
    mantissa = StandardComplex { re, im };
    exponent = 0;
}

ExtendedComplex::ExtendedComplex(const mpf_class &re, const mpf_class &im)
{
    long exp1, exp2;

    double mantissa1 = mpf_get_d_2exp(&exp1, re.get_mpf_t());
    double mantissa2 = mpf_get_d_2exp(&exp2, im.get_mpf_t());

    if (exp1 < exp2) {

        mantissa = StandardComplex { ldexp(mantissa1, (int)(exp1 - exp2)), mantissa2 };
        exponent = exp2;

    } else {

        mantissa = StandardComplex { mantissa1, ldexp(mantissa2, (int)(exp2 - exp1)) };
        exponent = exp1;
    }
}

ExtendedComplex::ExtendedComplex(const PrecisionComplex &other) :
ExtendedComplex(other.re, other.im) { }

/*
std::ostream& operator<<(std::ostream& os, const ExtendedComplex& value)
{
    
    os << "(" << value.mantissa.re;
    os << "," << value.mantissa.im << "i";
    os << ")" << "*2^" << value.exponent;

    return os;
}
*/

}
