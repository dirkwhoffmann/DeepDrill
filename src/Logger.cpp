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

namespace dd {

Logger&
Logger::operator<<(const log::Endl &arg)
{
    blanks++;
    std::cout << std::endl;
    return *this;
}

Logger&
Logger::operator<<(const log::VSpace &arg)
{
    if (blanks < 2) *this << log::endl;
    if (blanks < 2) *this << log::endl;
    return *this;
}

Logger&
Logger::operator<<(const log::ralign &arg)
{
    std::cout << std::right << std::setw(30) << arg.str;
    return *this;
}

Logger&
Logger::operator<<(const string &arg)
{
    blanks = 0;
    std::cout << arg;
    return *this;
}

Logger&
Logger::operator<<(const isize &arg)
{
    blanks = 0;
    std::cout << arg;
    return *this;
}

Logger&
Logger::operator<<(const Time &arg)
{
    blanks = 0;
    std::cout << arg;
    return *this;
}

    Logger log::cout;
}
