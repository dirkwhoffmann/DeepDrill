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

    Exception(const string &s, i64 d) : description(s), data(d) { }
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

struct InvalidValueException : public Exception {

    InvalidValueException(const string &s, i64 d) : Exception(s, d) {
        description = s + ": Invalid value (" + std::to_string(data) + ")";
    }
};

struct UserInterruptException : Exception {
    using Exception::Exception;
};

struct ScriptException : Exception {

    Exception exception;
    string file;
    isize line;

    ScriptException(const Exception &exception, const string &file, isize line) :
    exception(exception), file(file), line(line) { }

    void what(const class Logger &logger) const override;
};

}
