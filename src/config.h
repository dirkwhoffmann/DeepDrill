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
#define VER_MAJOR       3
#define VER_MINOR       2
#define VER_SUBMINOR    0

#define VER_BETA        1

// Mapfile format
#define MAP_FORMAT      320

// Uncomment this setting in a release build
#define RELEASEBUILD


//
// Constants
//

static const int MIN_MAP_WIDTH      = 256;
static const int MIN_MAP_HEIGHT     = 144;

static const int MAX_MAP_WIDTH      = 7680;
static const int MAX_MAP_HEIGHT     = 4320;

static const int MIN_IMAGE_WIDTH    = 256;
static const int MIN_IMAGE_HEIGHT   = 144;

static const int MAX_IMAGE_WIDTH    = 7680;
static const int MAX_IMAGE_HEIGHT   = 4320;


//
// Debug settings
//

static const int REC_DEBUG          = 0;    // Video recording
static const int GPU_DEBUG          = 0;    // GPU filters


//
// Forced error conditions
//

static const int FORCE_NO_FFMPEG    = 0;


//
// Build settings
//

#ifdef RELEASEBUILD
#ifndef NDEBUG
#define NDEBUG
#endif
static const int releaseBuild = 1;
#else
#ifdef NDEBUG
#undef NDEBUG
#endif
static const int releaseBuild = 0;
#endif

#if VER_BETA == 0
static const bool betaRelease = 0;
#else
static const bool betaRelease = 1;
#endif

#include <assert.h>
