/// -----------------------------------------------------------------------------
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
#include "Colorizer.h"
#include "Coord.h"
#include "IO.h"
#include "MapFile.h"
#include <iostream>

namespace dd {

Colorizer::Colorizer(const Options &o, const MapFile &m) : opt(o), map(m)
{
    palette.init(opt.palette);
    
    auto bytes = map.width * map.height;
    image = new u32[bytes] {};
}

Colorizer::~Colorizer()
{
    if (image) delete [] image;
}

void
Colorizer::colorize()
{
    printf("Colorizer::colorize()\n");
    
    for (isize y = 0; y < map.height; y++) {
        for (isize x = 0; x < map.width; x++) {
            colorize(Coord(x,y));
        }
    }
}

void
Colorizer::colorize(Coord c)
{
    auto data = map.get(c.x, c.y);
                        
    isize index = (isize)((data.iteration - log2(data.lognorm)) * 5);
    image[c.y * map.width + c.x] = palette.colorize(index);
}

void
Colorizer::save(const string &path)
{
    printf("Colorizer::save(%s)\n", path.c_str());
    
    std::ofstream os;
    
    // Assemble the target file names
    string rawFile = stripSuffix(path) + ".raw";
    string tifFile = path;
    
    std::cout << "rawFile: " << rawFile << std::endl;
    std::cout << "tifFile: " << tifFile << std::endl;

    // Open an output stream
    os.open(rawFile.c_str());
    
    // Dump texture
    for (int y = 0; y < opt.height; y++) {
        
        for (int x = 0; x < opt.width; x++) {
            
            char *cptr = (char *)(image + y * map.width + x);
            os.write(cptr + 0, 1);
            os.write(cptr + 1, 1);
            os.write(cptr + 2, 1);
        }
    }
    os.close();
    
    // Convert raw data into a TIFF file
    string cmd = "/usr/local/bin/raw2tiff";
    cmd += " -p rgb -b 3";
    cmd += " -w " + std::to_string(opt.width);
    cmd += " -l " + std::to_string(opt.height);
    cmd += " " + rawFile + " " + tifFile;
    
    // std::cout << "Executing " << cmd << std::endl;
    if (system(cmd.c_str()) != 0) {
        std::cout << "Failed to execute " << cmd << std::endl;
    }
}

}
