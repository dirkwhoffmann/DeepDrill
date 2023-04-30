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

#include "AssetManager.h"
#include "Exception.h"
#include "IO.h"
#include "Logger.h"

#include <SFML/Graphics.hpp>

namespace dd {

AssetManager::AssetManager()
{
    // Search assets in the repo directory by default
    auto repo = path(__FILE__).parent_path().parent_path().parent_path();

    paths.push_back(repo / "locations");
    paths.push_back(repo / "tutorial");
    paths.push_back(repo / "profiles");
    paths.push_back(repo / "palettes");
    paths.push_back(repo / "shaders");
}

void
AssetManager::addSearchPath(const path &path)
{
    paths.insert(paths.end(), path);
}


Format
AssetManager::getFormat(const string &path) {

    if (isDirectory(path)) return Format::DIR;

    auto suffix = extractSuffix(path);
    if (suffix == "bmp") return Format::BMP;
    if (suffix == "glsl") return Format::GLSL;
    if (suffix == "jpg") return Format::JPG;
    if (suffix == "loc") return Format::LOC;
    if (suffix == "map") return Format::MAP;
    if (suffix == "mpg" || suffix == "mov") return Format::MPG;
    if (suffix == "png") return Format::PNG;
    if (suffix == "prf") return Format::PRF;
    if (suffix == "prj") return Format::PRJ;

    return Format::NONE;
}

void
AssetManager::assureFormat(const path &name, Format format)
{
    assureFormat(name, vector <Format> { format });
}

void
AssetManager::assureFormat(const path &name, vector <Format> formats)
{
    auto format = getFormat(name);

    if (std::find(formats.begin(), formats.end(), format) == formats.end()) {

        vector <string> s;
        for (const auto &it : formats) {

            switch(it) {

                case Format::BMP: s.push_back(".bmp"); break;
                case Format::GLSL: s.push_back(".glsl"); break;
                case Format::JPG: s.push_back(".jpg"); break;
                case Format::LOC: s.push_back(".loc"); break;
                case Format::MAP: s.push_back(".map"); break;
                case Format::MPG: s.push_back(".mpg"); s.push_back(".mov"); break;
                case Format::PNG: s.push_back(".png"); break;
                case Format::PRF: s.push_back(".prf"); break;
                case Format::PRJ: s.push_back(".prj"); break;

                default:
                    assert(false);
            }
        }
        throw Exception("'" + name.string() + "' has an invalid file extension. Expected " + join(s, ",", ", or"));
    }
}

bool
AssetManager::isImageFormat(Format format) {

    return
    format == Format::BMP ||
    format == Format::JPG ||
    format == Format::PNG;
}

bool
AssetManager::isVideoFormat(Format format) {

    return
    format == Format::MPG;
}

path
AssetManager::findAsset(const path &name) const
{
    if (name != "") {

        // Search the file at the specified path
        if (fileExists(name)) return name;

        // Search the file in the assets directories
        for (const auto &it : paths) {
            if (fileExists(it / name)) return it / name;
        }
    }

    // File not found
    throw FileNotFoundError(name);
}

path
AssetManager::findAsset(const path &name, Format format) const
{
    return findAsset(name, vector <Format> { format });
}

path
AssetManager::findAsset(const path &name, vector <Format> formats) const
{
    assureFormat(name, formats);
    return findAsset(name);
}

}
