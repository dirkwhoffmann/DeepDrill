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

// enum class Format { NONE, BMP, DIR, GLSL, INI, JPG, LOC, MAP, MPG, PNG, PRF, PRJ };
enum class Format { NONE, BMP, DIR, GLSL, INI, JPG, MAP, MPG, PNG };

class AssetManager {

    // Assets search paths
    std::vector<fs::path> paths;

public:

    static Format getFormat(const string &path);
    static void assureFormat(const fs::path &name, Format format);
    static void assureFormat(const fs::path &name, std::vector<Format> formats);
    static bool isImageFormat(Format format);
    static bool isVideoFormat(Format format);


    //
    // Initialization
    //

public:

    AssetManager();

    void addSearchPath(const fs::path &path);


    //
    // Locating files
    //

public:

    fs::path findAsset(const fs::path &name) const;
    fs::path findAsset(const fs::path &name, Format format) const;
    fs::path findAsset(const fs::path &name, std::vector<Format> formats) const;
};

}
