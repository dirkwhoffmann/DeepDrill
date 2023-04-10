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
    
    string description;
    i64 data;

    Exception(const string &s, i64 d) : data(d), description(s) { }
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

struct InvalidValueException : public Exception {

    InvalidValueException(const string &s, i64 d) : Exception(s, d) {
        description = s + ": Invalid value (" + std::to_string(data) + ")";
    }
};

struct UserInterruptException : Exception {
    using Exception::Exception;
};

}
