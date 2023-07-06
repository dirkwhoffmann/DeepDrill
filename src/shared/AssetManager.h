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

class AssetManager {

    // Assets search paths
    static std::vector<fs::path> paths;

public:

    // This class is not meant to be instantiated
    AssetManager() = delete;

    // Managing file names
    static fs::path iniFile() { return "deepzoom.ini"; }
    static fs::path movFile() { return "deepzoom.mov"; }
    static fs::path rawFile(isize i) { return "keyframe_" + std::to_string(i); }
    static fs::path iniFile(isize i) { return rawFile(i).string() + ".ini"; }
    static fs::path mapFile(isize i) { return rawFile(i).string() + ".map"; }
    static fs::path imgFile(isize i) { return rawFile(i).string() + ".jpg"; }

    // Managing file formats
    static Format getFormat(const fs::path &path);
    static void assureFormat(const fs::path &name, Format format);
    static void assureFormat(const fs::path &name, std::vector<Format> formats);
    static bool isImageFormat(Format format);
    static bool isVideoFormat(Format format);


    //
    // Initialization
    //

public:

    static void addSearchPath(const fs::path &path);


    //
    // Locating asset files
    //

public:

    static fs::path findAsset(const fs::path &name);
    static fs::path findAsset(const fs::path &name, Format format);
    static fs::path findAsset(const fs::path &name, std::vector<Format> formats);
};

}
