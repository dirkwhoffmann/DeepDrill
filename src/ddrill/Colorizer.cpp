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
#include "Logger.h"
#include "DrillOptions.h"
#include "ProgressIndicator.h"

namespace dd {

Colorizer::Colorizer(const DrillOptions &o, const DrillMap &m) : opt(o), map(m)
{
    palette.init(opt.palette.values);
    
    auto bytes = map.width * map.height;
    image = new u32[bytes] {};
    string paths[] = { "/usr/bin/raw2tiff", "/usr/local/bin/raw2tiff", "/opt/homebrew/bin/raw2tiff" };

    for (isize i = 0; i < 3; i++) {

        raw2tiff = paths[i];
        if (fileExists(raw2tiff)) return;
    }

    throw Exception("Expecting raw2tiff in /" + paths[0] + ", " + paths[1] + ", or " + paths[2]);
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
        
        if (opt.stop) throw UserInterruptException();
        progress.step(map.width);
    }
}

void
Colorizer::colorize(Coord c)
{
    auto data = map.get(c.x, c.y);

    // Map to a black if the point belongs to the mandelbrot set
    if (data.iteration == 0) {

        image[c.y * map.width + c.x] = 0;
        return;
    }
    
    isize index = (isize)((data.iteration - log2(data.lognorm)) * 5);
    image[c.y * map.width + c.x] = palette.colorize(index);
}

void
Colorizer::save(const string &path)
{
    namespace fs = std::filesystem;
    fs::path tmp = fs::temp_directory_path();

    ProgressIndicator progress("Saving image data");
    
    std::ofstream os;
        
    isize width = map.width;
    isize height = map.height;
    
    // Assemble the target file names
    string rawFile = tmp / "image.raw";
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
    string options = "-p rgb -b 3";
    options += " -w " + std::to_string(width);
    options += " -l " + std::to_string(height);
    string command = raw2tiff + " " + options + " " + rawFile + " " + tifFile;
    
    if (system(command.c_str()) != 0) {
        throw Exception("Failed to execute " + command);
    }
    
    // Remove the temporary file
    fs::remove(rawFile);
    
    progress2.done();
    
    if (opt.verbose) {
        
        log::cout << log::vspace;
        log::cout << log::ralign("Input: ");
        log::cout << rawFile << log::endl;
        log::cout << log::ralign("Output: ");
        log::cout << tifFile << log::endl;
        log::cout << log::ralign("Converter: ");
        log::cout << raw2tiff << log::endl;
        log::cout << log::ralign("Options: ");
        log::cout << options << log::vspace;
    }
}

}
