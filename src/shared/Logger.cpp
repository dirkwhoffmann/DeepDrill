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
    if (verbose()) {
        
        blanks++;
        stream << std::endl;
    }
    return *this;
}

Logger&
Logger::operator<<(const log::VSpace &arg)
{
    if (verbose()) {

        if (blanks < 2) *this << log::endl;
        if (blanks < 2) *this << log::endl;
    }
    return *this;
}

Logger&
Logger::operator<<(const log::Flush &arg)
{
    if (verbose()) {

        stream.flush();
    }
    return *this;
}

Logger&
Logger::operator<<(const log::ralign &arg)
{
    if (verbose()) {

        blanks = 0;
        stream << std::right << std::setw(arg.w) << arg.s;
    }
    return *this;
}

Logger&
Logger::operator<<(const string &arg)
{
    if (verbose()) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const fs::path &arg)
{
    *this << arg.string();
    return *this;
}

Logger&
Logger::operator<<(const char &arg)
{
    if (verbose()) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const char *arg)
{
    if (verbose()) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const bool &arg)
{
    if (verbose()) {

        blanks = 0;
        stream << (arg ? "yes" : "no");
    }
    return *this;
}

Logger&
Logger::operator<<(const isize &arg)
{
    if (verbose()) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const usize &arg)
{
    if (verbose()) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const double &arg)
{
    if (verbose()) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const Time &arg)
{
    if (verbose()) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const Coord &arg)
{
    if (verbose()) {

        blanks = 0;
        stream << "(" << arg.x << "," << arg.y << ")";
    }
    return *this;
}

Logger&
Logger::operator<<(const StandardComplex& arg)
{
    if (verbose()) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const ExtendedDouble& arg)
{
    if (verbose()) {

        blanks = 0;
        stream << arg.mantissa << "b" << arg.exponent;
    }
    return *this;
}

Logger&
Logger::operator<<(const ExtendedComplex& arg)
{
    if (verbose()) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const PrecisionComplex& arg)
{
    if (verbose()) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

Logger&
Logger::operator<<(const Exception& arg)
{
    arg.what(*this);
    return *this;
}

Logger&
Logger::operator<<(const Dynamic& arg)
{
    if (verbose()) {

        blanks = 0;
        stream << arg;
    }
    return *this;
}

bool
Logger::verbose() const
{
    return !muted && (!id || std::this_thread::get_id() == id);
}


Logger log::cout(std::cout);

}
