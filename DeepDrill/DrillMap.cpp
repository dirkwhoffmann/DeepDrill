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
#include "ProgressIndicator.h"
#include "ReferencePoint.h"
#include <fstream>
#include <iostream>

namespace dd {

DrillMap::DrillMap(const Options &options) : opt(options)
{
    if (isize bytes = opt.width * opt.height) {

        if (image) delete [] image;
        image = new u32[bytes] {};
    }
}

DrillMap::~DrillMap()
{
    if (image) delete [] image;
}

void
DrillMap::saveImage()
{
    save(opt.exportPath + "/" + opt.exportName, image);
}

void
DrillMap::saveGlitchImage(const vector<Coord> &glitches, isize round)
{
    // Allocate image date
    u32 *data = new u32[opt.width * opt.height] {};
                         
    // Colorize glitched pixels
    for (auto &it : glitches) data[it.y * opt.width + it.x] = 0xFF;
    
    // Assemble the file names
    string name = "glitch_" + std::to_string(round);

    // Save the image
    save(name, data);
}

void
DrillMap::save(const string &name, u32 *data) {
 
    std::ofstream os;
        
    // Assemble the target file names
    string rawFile = name + ".raw";
    string tiffFile = name + ".tiff";

    // Open an output stream
    os.open(rawFile.c_str());
    
    // Dump texture
    for (int y = 0; y < opt.height; y++) {

        for (int x = 0; x < opt.width; x++) {
            
            char *cptr = (char *)(data + y * opt.width + x);
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
    cmd += " " + rawFile + " " + tiffFile;
    
    // std::cout << "Executing " << cmd << std::endl;
    system(cmd.c_str());
}

}
