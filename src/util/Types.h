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

//
// Integers
//

#include <sys/types.h>
#include <climits>

// Signed integers
typedef signed char        i8;
typedef signed short       i16;
typedef signed int         i32;
typedef signed long long   i64;
typedef ssize_t            isize;

// Unsigned integers
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef size_t             usize;

// Signed alternative for the sizeof keyword
#define isizeof(x) (isize)(sizeof(x))


//
// Strings
//

#include <string>
using std::string;


//
// Files
//
#include <filesystem>
namespace fs = std::filesystem;

enum class Format
{
    NONE,       // Unknown file format
    BMP,        // Image file
    DIR,        // Directory
    GLSL,       // GPU shader
    INI,        // Configuration file
    JPG,        // Image format
    MAP,        // Proprietary map file format
    MPG,        // Video file
    PNG         // Image file
};


//
// Data structures
//

#include <vector>
#include <map>
