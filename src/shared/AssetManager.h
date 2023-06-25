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

class AssetManager {

    // Assets search paths
    std::vector<fs::path> paths;

public:

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

    AssetManager();

    void addSearchPath(const fs::path &path);


    //
    // Locating asset files
    //

public:

    fs::path findAsset(const fs::path &name) const;
    fs::path findAsset(const fs::path &name, Format format) const;
    fs::path findAsset(const fs::path &name, std::vector<Format> formats) const;
};

}