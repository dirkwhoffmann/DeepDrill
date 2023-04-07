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
#include "DrillMap.h"
#include "Coord.h"
#include "Logger.h"
#include "Options.h"
#include "ProgressIndicator.h"

namespace dd {

DrillMap::DrillMap(const Options &o) : opt(o)
{
    resize(opt.image.width, opt.image.height);
}

void
DrillMap::load(const string &path)
{
    std::ifstream os(path.c_str(), std::ios::binary);
    if (!os.is_open()) throw Exception("Failed to open file " + path);
 
    ProgressIndicator progress("Loading map file");

    // Read header
    os.read((char *)&width, sizeof(width));
    os.read((char *)&height, sizeof(height));
    os.read((char *)&logBailout, sizeof(logBailout));
    
    // Adjust the map size
    resize(width, height);
    
    // Write data
    for (isize y = 0; y < height; y++) {
        for (isize x = 0; x < width; x++) {
            
            u32 iteration;
            float lognorm;
            
            os.read((char *)&iteration, sizeof(iteration));
            os.read((char *)&lognorm, sizeof(lognorm));

            set(x, y, MapEntry { iteration, lognorm });
        }
    }
    
    progress.done();
    
    if (opt.verbose) {
        
        log::cout << log::vspace;
        log::cout << log::ralign("Image size: ");
        log::cout << "(" << width << "," << height << ")" << log::endl;
        log::cout << log::vspace;
    }
}

void
DrillMap::resize(isize w, isize h)
{
    assert(w <= 3840);
    assert(h <= 2160);

    if (data) delete [] data;
    
    width = w;
    height = h;
    data = new MapEntry[w * h];
}

const MapEntry &
DrillMap::get(isize w, isize h) const
{
    assert(data != nullptr && w < width && h < height);
    return data[h * width + w];
}

const MapEntry &
DrillMap::get(const struct Coord &c) const
{
    return get(c.x, c.y);
}

void
DrillMap::set(isize w, isize h, const MapEntry &entry)
{
    assert(data != nullptr && w < width && h < height);
    data[h * width + w] = entry;
}

void
DrillMap::set(const struct Coord &c, const MapEntry &entry)
{
    set(c.x, c.y, entry);
}

void
DrillMap::save(const string &path)
{
    ProgressIndicator progress("Saving map file");
        
    std::ofstream os(path.c_str(), std::ios::binary);
    if (!os.is_open()) throw Exception("Failed to write file " + path);
    save(os);
}

void
DrillMap::save(std::ostream &os)
{
    // Write header
    os.write((char *)&width, sizeof(width));
    os.write((char *)&height, sizeof(height));
    os.write((char *)&logBailout, sizeof(logBailout));

    // Write data
    for (isize y = 0; y < height; y++) {
        for (isize x = 0; x < width; x++) {
            
            auto item = get(x, y);
            os.write((char *)&item.iteration, sizeof(item.iteration));
            os.write((char *)&item.lognorm, sizeof(item.lognorm));
        }
    }
}

}
