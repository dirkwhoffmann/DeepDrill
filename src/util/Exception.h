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
#include "IO.h"

namespace dd {

struct Exception : public std::exception {
    
    string description;
    i64 data;

    // Exception(const string &s, i64 d) : description(s), data(d) { }
    Exception(const string &s) : description(s), data(0) { }
    Exception(i64 d) : description(""), data(d) { }
    Exception() : data(0) { }
    
    const char *what() const throw() override { return description.c_str(); }
    virtual void what(const class Logger &logger) const;
};

struct SyntaxError : public Exception {
    using Exception::Exception;
};

struct KeyValueError : public Exception {

    KeyValueError(const string &k, const string &s) {
        description = "'" + k + "': " + s;
    }
};

struct FileNotFoundError : Exception {

    FileNotFoundError(const string &s) {
        description = s + ": File not found.";
    }
};

/*
struct InvalidValueException : public Exception {

    InvalidValueException(const string &s, i64 d) : Exception(s, d) {
        description = s + ": Invalid value (" + std::to_string(data) + ")";
    }
};
*/

struct UserInterruptException : Exception {
    using Exception::Exception;
};

struct ParseError : Exception {

    // Wrapped exception
    Exception exception;

    // Line in which the error occurred
    isize line;

    // File in which the error occurred
    string file;

    ParseError(const Exception &exception, isize line) :
    exception(exception), line(line), file("") { }
    ParseError(const Exception &exception,  isize line, const string &file) :
    exception(exception), line(line), file(file) { }

    void what(const class Logger &logger) const override;
};

}
