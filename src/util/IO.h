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
#include <dirent.h>
#include <fcntl.h>
#include <istream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace dd {

//
// Handling file names
//

// Changes the capitalization of a string
string lowercased(const string& s);
string uppercased(const string& s);

// Extracts a certain component from a path
// string extractPath(const string &path);
// string extractName(const string &path);
// string extractSuffix(const string &path);

// Strips a certain component from a path
// string stripPath(const string &path);
// string stripName(const string &path);
// string stripSuffix(const string &path);

// Joins a vector of strings
string join(const std::vector<string> &v, const string &delim1);
string join(const std::vector<string> &v, const string &delim1, const string &delim2);

// Counts files in a directory
isize countFiles(const fs::path &suffix);
isize countFiles(const fs::path &path, const fs::path &suffix);


//
// Handling files
//

// Checks the header signature (magic bytes) of a stream or buffer
bool matchingStreamHeader(std::istream &stream, const u8 *header, isize len);
bool matchingBufferHeader(const u8 *buffer, const u8 *header, isize len);

// Compares two files
bool compareFiles(const fs::path &path1, const fs::path &path2);


//
// Handling streams
//

struct dec {
    
    i64 value;
    
    dec(i64 v) : value(v) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct hex {
    
    int digits;
    u64 value;
    
    hex(int d, u64 v) : digits(d), value(v) { };
    hex(u64 v) : hex(16, v) { };
    hex(u32 v) : hex(8, v) { };
    hex(u16 v) : hex(4, v) { };
    hex(u8 v) : hex(2, v) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct tab {
    
    int pads;
    const string &str;
    
    tab(int p, const string &s) : pads(p), str(s) { };
    tab(const string &s) : tab(24, s) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct bol {
    
    static const string& yes;
    static const string& no;
    
    bool value;
    const string &str1, &str2;
    
    bol(bool v, const string &s1, const string &s2) : value(v), str1(s1), str2(s2) { };
    bol(bool v) : bol(v, yes, no) { };
    std::ostream &operator()(std::ostream &os) const;
};

inline std::ostream &operator <<(std::ostream &os, dec v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, hex v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, tab v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, bol v) { return v(os); }

}
