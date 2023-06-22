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

    // Textual description of this exception
    string desc;

    // An additional integer payload
    i64 data;

    Exception(const string &desc, i64 data) : desc(desc), data(data) { }
    Exception(const string &desc) : desc(desc), data(0) { }
    Exception(i64 data) : desc(""), data(data) { }
    Exception() : desc(""), data(0) { }
    
    const char *what() const throw() override { return desc.c_str(); }
    virtual void what(const class Logger &logger) const;
};

struct SyntaxError : public Exception {

    using Exception::Exception;
};

struct KeyValueError : public Exception {

    KeyValueError(const string &k, const string &s) {
        desc = "'" + k + "': " + s;
    }
};

struct FileNotFoundError : Exception {

    FileNotFoundError(const string &s) {
        desc = s + ": File not found.";
    }
};

struct UserInterruptException : Exception {

    using Exception::Exception;

    void what(const class Logger &logger) const override;
};

struct ParseError : Exception {

    // Embedded (other) exception
    Exception exception;

    // Line in which the error occurred
    isize line;

    // File in which the error occurred
    fs::path path;

    ParseError(const Exception &exception, isize line) :
    exception(exception), line(line), path("") { }
    ParseError(const Exception &exception,  isize line, const fs::path &path) :
    exception(exception), line(line), path(path) { }

    void what(const class Logger &logger) const override;
};

}
