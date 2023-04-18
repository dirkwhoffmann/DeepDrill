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
#include "Logger.h"
#include "Coord.h"

namespace dd {

Logger&
Logger::operator<<(const log::Endl &arg)
{
    if (!silent) {
        
        blanks++;
        stream << std::endl;
    }
    return *this;
}

Logger&
Logger::operator<<(const log::VSpace &arg)
{
    if (!silent) {

        if (blanks < 2) *this << log::endl;
        if (blanks < 2) *this << log::endl;
    }
    return *this;
}

Logger&
Logger::operator<<(const log::Flush &arg)
{
    if (!silent) {

        stream.flush();
    }
    return *this;
}

Logger&
Logger::operator<<(const log::ralign &arg)
{
    if (!silent) {

        blanks = 0;
        stream << std::right << std::setw(30) << arg.str;
    }
    return *this;
}

Logger&
Logger::operator<<(const string &arg)
{
    if (!silent) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const isize &arg)
{
    if (!silent) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const Time &arg)
{
    if (!silent) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const Coord &arg)
{
    if (!silent) {

        blanks = 0;
        stream << "(" << arg.x << "," << arg.y << ")";
    }
    return *this;
}

Logger&
Logger::operator<<(const StandardComplex& arg)
{
    if (!silent) {

        blanks = 0;
        stream << "(" << arg.re;
        stream << "," << arg.im << "i";
        stream << ")";
    }
    return *this;
}

Logger&
Logger::operator<<(const ExtendedDouble& arg)
{
    if (!silent) {

        blanks = 0;
        stream << arg.mantissa << "b" << arg.exponent;
    }
    return *this;

}

Logger&
Logger::operator<<(const ExtendedComplex& arg)
{
    if (!silent) {

        blanks = 0;
        stream << "(" << arg.mantissa.re;
        stream << "," << arg.mantissa.im << "i";
        stream << ")" << "b" << arg.exponent;
    }
    return *this;
}

Logger&
Logger::operator<<(const PrecisionComplex& arg)
{
    if (!silent) {

        blanks = 0;
        stream << "(" << arg.re;
        stream << "," << arg.im << "i";
        stream << ")";
    }
    return *this;
}

void
Logger::setColor(isize c, bool b)
{
    color = c;
    bold = b;

    *this << "\033[" << (bold ? "1" : "0") << ";3" << color << "m";
}

Logger log::cout(std::cout);

}
