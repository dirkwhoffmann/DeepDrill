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

#include "Logger.h"
#include "Coord.h"
#include "IO.h"

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
        stream << std::right << std::setw(32) << arg.str;
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
Logger::operator<<(const char &arg)
{
    if (!silent) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const char *arg)
{
    if (!silent) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const bool &arg)
{
    if (!silent) {

        blanks = 0;
        stream << (arg ? "yes" : "no");
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
Logger::operator<<(const usize &arg)
{
    if (!silent) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const double &arg)
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
        stream << arg;
        /*
        stream << "(" << arg.re;
        stream << "," << arg.im << "i";
        stream << ")";
        */
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
        stream << arg;
        /*
        stream << "(" << arg.mantissa.re;
        stream << "," << arg.mantissa.im << "i";
        stream << ")" << "b" << arg.exponent;
        */
    }
    return *this;
}

Logger&
Logger::operator<<(const PrecisionComplex& arg)
{
    if (!silent) {

        blanks = 0;
        stream << arg;
        /*
        stream << "(" << arg.re;
        stream << "," << arg.im << "i";
        stream << ")";
        */
    }
    return *this;
}

Logger&
Logger::operator<<(const Exception& arg)
{
    arg.what(*this);
    return *this;
}

Logger log::cout(std::cout);

}
