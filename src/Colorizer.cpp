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
#include "Colorizer.h"
#include "Coord.h"
#include "DrillMap.h"
#include "Options.h"
#include "ProgressIndicator.h"

namespace dd {

Colorizer::Colorizer(const Options &o, const DrillMap &m) : opt(o), map(m)
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
    ProgressIndicator progress("Colorizing", map.height * map.width);
    
    for (isize y = 0; y < map.height; y++) {
        
        for (isize x = 0; x < map.width; x++) {
            colorize(Coord(x,y));
        }
        progress.step(map.width);
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
    ProgressIndicator progress("Saving image");
    
    std::ofstream os;
    
    isize width = map.width;
    isize height = map.height;
    
    // Assemble the target file names
    string rawFile = stripSuffix(path) + ".raw";
    string tifFile = path;
    
    // Open an output stream
    os.open(rawFile.c_str());
    
    // Dump texture
    for (int y = 0; y < height; y++) {
        
        for (int x = 0; x < width; x++) {
            
            char *cptr = (char *)(image + y * width + x);
            os.write(cptr + 0, 1);
            os.write(cptr + 1, 1);
            os.write(cptr + 2, 1);
        }
    }
    os.close();
    progress.done();

    ProgressIndicator progress2("Converting to TIFF");
    
    // Convert raw data into a TIFF file
    string exec = "/usr/local/bin/raw2tiff";
    string options = "-p rgb -b 3";
    options += " -w " + std::to_string(width);
    options += " -l " + std::to_string(height);
    string command = exec + " " + options + " " + rawFile + " " + tifFile;
    
    // std::cout << "Executing " << cmd << std::endl;
    if (system(command.c_str()) != 0) {
        throw Exception("Failed to execute " + command);
    }
    progress2.done();
    
    if (opt.verbose) {
        
        std::cout << std::endl;
        std::cout << RALIGN << "Input: ";
        std::cout << rawFile << std::endl;
        std::cout << RALIGN << "Output: ";
        std::cout << tifFile << std::endl;
        std::cout << RALIGN << "Converter: ";
        std::cout << exec << std::endl;
        std::cout << RALIGN << "Options: ";
        std::cout << options << std::endl;
    }
}

}
