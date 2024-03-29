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

namespace dd {

std::vector<fs::path> AssetManager::paths = [](){

    // Search assets in the repo directory by default
    auto repo = fs::path(__FILE__).parent_path().parent_path().parent_path();

    std::vector<fs::path> paths;
    paths.push_back(repo / "locations");
    paths.push_back(repo / "locations" / "yarndley");
    paths.push_back(repo / "locations" / "wiki");
    paths.push_back(repo / "tutorial");
    paths.push_back(repo / "profiles");
    paths.push_back(repo / "palettes");
    paths.push_back(repo / "textures");
    paths.push_back(repo / "shaders" / "colorizers");
    paths.push_back(repo / "shaders" / "illuminators");
    paths.push_back(repo / "shaders" / "scalers");
    paths.push_back(repo / "shaders" / "experimental");

    return paths;
}();

void
AssetManager::addSearchPath(const fs::path &path)
{
    paths.insert(paths.end(), path);
}


Format
AssetManager::getFormat(const fs::path &path) {

    auto ext = path.extension();

    if (ext == ".bmp") return Format::BMP;
    if (ext == ".glsl") return Format::GLSL;
    if (ext == ".ini") return Format::INI;
    if (ext == ".jpg") return Format::JPG;
    if (ext == ".map") return Format::MAP;
    if (ext == ".mpg" || ext == ".mov") return Format::MPG;
    if (ext == ".png") return Format::PNG;
    if (fs::is_directory(path)) return Format::DIR;

    return Format::NONE;
}

void
AssetManager::assureFormat(const fs::path &name, Format format)
{
    assureFormat(name, std::vector<Format> { format });
}

void
AssetManager::assureFormat(const fs::path &name, std::vector<Format> formats)
{
    auto format = getFormat(name);

    if (std::find(formats.begin(), formats.end(), format) == formats.end()) {

        std::vector<string> s;
        for (const auto &it : formats) {

            switch(it) {

                case Format::BMP: s.push_back(".bmp"); break;
                case Format::GLSL: s.push_back(".glsl"); break;
                case Format::INI: s.push_back(".ini"); break;
                case Format::JPG: s.push_back(".jpg"); break;
                case Format::MAP: s.push_back(".map"); break;
                case Format::MPG: s.push_back(".mpg"); s.push_back(".mov"); break;
                case Format::PNG: s.push_back(".png"); break;

                default:
                    assert(false);
            }
        }
        throw Exception("'" + name.string() + "'" +
                        " has an invalid file extension." +
                        " Expected " + join(s, ", ", ", or "));
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

fs::path
AssetManager::findAsset(const fs::path &name)
{
    if (name != "") {

        // Search the file at the specified path
        if (fs::exists(name)) return name;

        // Search the file in the assets directories
        for (const auto &it : paths) {
            if (fs::exists(it / name)) return it / name;
        }
    }

    throw FileNotFoundError(name);
}

fs::path
AssetManager::findAsset(const fs::path &name, Format format)
{
    return findAsset(name, std::vector<Format> { format });
}

fs::path
AssetManager::findAsset(const fs::path &name, std::vector<Format> formats)
{
    assureFormat(name, formats);
    return findAsset(name);
}

}
