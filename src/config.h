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


//
// Release settings
//

// Version number
#define VER_MAJOR 0
#define VER_MINOR 3

// Uncomment this setting in a release build
// #define RELEASEBUILD


//
// Debug settings
//

static const int REC_DEBUG          = 0; // Video recording

//
// Forced error conditions
//

static const int FORCE_NO_FFMPEG   = 0;


//
// Build settings
//

#ifdef RELEASEBUILD
#define NDEBUG
static const int releaseBuild = 1;
#else
static const int releaseBuild = 0;
#endif

#include <assert.h>
