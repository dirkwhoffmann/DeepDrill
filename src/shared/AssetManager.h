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

enum class Format { NONE, BMP, DIR, GLSL, JPG, LOC, MAP, MPG, PNG, PRF, PRJ };

class AssetManager {

    // Assets search paths
    vector <path> paths;

public:

    static Format getFormat(const string &path);
    static void assureFormat(const path &name, Format format);
    static void assureFormat(const path &name, vector <Format> formats);
    static bool isImageFormat(Format format);
    static bool isVideoFormat(Format format);


    //
    // Initialization
    //

public:

    AssetManager();

    void addSearchPath(const path &path);


    //
    // Locating files
    //

public:

    path findAsset(const path &name) const;
    path findAsset(const path &name, Format format) const;
    path findAsset(const path &name, vector <Format> formats) const;
};

}
