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

#include "config.h"
#include "MapFile.h"

namespace dd {

MapFile::MapFile(isize w, isize h)
{
    resize(w, h);
}

MapFile::MapFile(const string &path)
{
    std::ifstream os(path.c_str());
    if (!os.is_open()) throw Exception("Failed to open file " + path);

    // Read header
    os >> width;
    os >> height;
    os >> logBailout;

    // Write data
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            
            u32 iteration;
            float lognorm;
            
            os >> iteration;
            os >> lognorm;

            set(x, y, MapEntry { iteration, lognorm });
        }
    }
}

void
MapFile::resize(isize w, isize h)
{
    assert(w <= 3840);
    assert(h <= 2160);

    if (data) delete [] data;
    
    width = w;
    height = h;
    data = new MapEntry[w * h];
}

const MapEntry &
MapFile::get(isize w, isize h)
{
    assert(data != nullptr && w < width && h < height);
    return data[h * width + w];
}

void
MapFile::set(isize w, isize h, const MapEntry &entry)
{
    assert(data != nullptr && w < width && h < height);
    data[h * width + w] = entry;
}

void
MapFile::save(const string &path)
{
    std::ofstream os(path.c_str());
    if (!os.is_open()) throw Exception("Failed to open file " + path);
    save(os);
}

void
MapFile::save(std::ostream &os)
{
    // Write header
    os << width;
    os << height;
    os << logBailout;

    // Write data
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            
            auto item = get(x,y);
            os << item.iteration;
            os << item.lognorm;
        }
    }
}

}
