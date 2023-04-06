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

#include "Types.h"

namespace dd {

struct Exception : public std::exception {
    
    i64 data;
    string description;
    
    Exception(i64 d, const string &s) : data(d), description(s) { }
    Exception(i64 d) : data(d), description("") { }
    Exception(const string &s) : data(0), description(s) { }
    Exception() : data(0) { }
    
    const char *what() const throw() override { return description.c_str(); }
};

struct SyntaxError : public Exception {
    using Exception::Exception;
};

struct FileNotFoundError : Exception {
    using Exception::Exception;
};

struct InvalidValueException : Exception {
    using Exception::Exception;
};

struct UserInterruptException : Exception {
    using Exception::Exception;
};

}
