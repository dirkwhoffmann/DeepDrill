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
#define VER_MAJOR 1
#define VER_MINOR 3
#define VER_BETA  1

// Uncomment this setting in a release build
// #define RELEASEBUILD


//
// Constants
//

static const int MIN_MAP_WIDTH      = 480;
static const int MIN_MAP_HEIGHT     = 270;

static const int MAX_MAP_WIDTH      = 7680;
static const int MAX_MAP_HEIGHT     = 4320;

static const int MIN_IMAGE_WIDTH    = 480;
static const int MIN_IMAGE_HEIGHT   = 270;

static const int MAX_IMAGE_WIDTH    = 3840;
static const int MAX_IMAGE_HEIGHT   = 2160;

static const int MIN_VIDEO_WIDTH    = 480;
static const int MIN_VIDEO_HEIGHT   = 270;

static const int MAX_VIDEO_WIDTH    = 1920;
static const int MAX_VIDEO_HEIGHT   = 1080;


//
// Debug settings
//

static const int REC_DEBUG          = 0; // Video recording


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
static const int releaseBuild = 0;
#endif

#if VER_BETA == 0
static const bool betaRelease = 0;
#else
static const bool betaRelease = 1;
#endif

#include <assert.h>
