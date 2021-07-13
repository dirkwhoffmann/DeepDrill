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

#include <sys/types.h>

//
// Integers
//

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
// Data structures
//

#include <vector>
#include <map>
using std::vector;
using std::map;


//
// Aliases
//

// typedef struct { i16 x; i16 y; } Coord;
typedef struct { i16 width; i16 height; } Size;

#define RALIGN std::right << std::setw(30)


//
// Syntactic sugar
//

/* The following keyword is used for documentary purposes only. It is used to
 * mark all methods that use the exception mechanism to signal error conditions
 * instead of returning error codes. It is used in favor of classic throw
 * lists, since the latter cause the compiler to embed unwanted runtime checks
 * in the code.
 */
#define throws
