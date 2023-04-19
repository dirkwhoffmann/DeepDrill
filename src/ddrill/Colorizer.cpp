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

#include "Colorizer.h"
#include "Coord.h"
#include "DrillMap.h"
#include "Exception.h"
#include "IO.h"
#include "Logger.h"
#include "Options.h"
#include "ProgressIndicator.h"

namespace dd {

Colorizer::Colorizer(const Options &o, const DrillMap &m) : opt(o), map(m)
{
    auto values = opt.palette.values;

    if (std::count(values.cbegin(), values.cend(), ' ') != 0) {

        // Custom palette
        scheme = ColorScheme::Custom;

        std::stringstream stream(values);
        u32 value;

        while(stream >> value) {
            colors.push_back(value);
        }

    } else {

        // Default palette
        scheme = ColorScheme::Default;
    }

    // Allocate image data
    image.alloc(map.width * map.height);
}

Colorizer::~Colorizer()
{

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

    switch (scheme) {

        case ColorScheme::Custom:
        {
            isize index = (isize)((data.iteration - log2(data.lognorm)) * 5);
            image[c.y * map.width + c.x] = colors[index % colors.size()];
            break;
        }
        case ColorScheme::Default:
        {
            // Adapted from https://www.shadertoy.com/view/tllSWj
            float sl = (float(data.iteration) - log2(data.lognorm)) + 4.0;
            sl *= .0025;

            float r = 0.5 + 0.5 * cos(2.7 + sl * 30.0 + 0.0);
            float g = 0.5 + 0.5 * cos(2.7 + sl * 30.0 + 0.6);
            float b = 0.5 + 0.5 * cos(2.7 + sl * 30.0 + 1.0);
            auto rr = u8(r * 255.0);
            auto gg = u8(g * 255.0);
            auto bb = u8(b * 255.0);

            image[c.y * map.width + c.x] = rr << 0 | gg << 8 | bb << 16 | 255 << 24;
            break;
        }
    }
}

void
Colorizer::save(const string &path)
{
    namespace fs = std::filesystem;
    fs::path tmp = fs::temp_directory_path();
    auto name = stripSuffix(stripPath(path));

    ProgressIndicator progress("Saving image data");
    
    std::ofstream os;
        
    isize width = map.width;
    isize height = map.height;
    
    // Assemble the target file names
    string rawFile, tifFile, pngFile;

    if (opt.outputFormat == Format::TIF) {

        rawFile = tmp / (name + ".raw");
        tifFile = path;
        pngFile = "";
    }
    if (opt.outputFormat == Format::PNG) {

        rawFile = tmp / (name + ".raw");
        tifFile = tmp / (name + ".tif");
        pngFile = path;
    }

    // Open an output stream
    os.open(rawFile.c_str());
    
    // Dump texture
    for (isize y = height - 1; y >= 0; y--) {
        
        for (isize x = 0; x < width; x++) {
            
            char *cptr = (char *)(image.ptr + y * width + x);
            os.write(cptr + 0, 1);
            os.write(cptr + 1, 1);
            os.write(cptr + 2, 1);
        }
    }
    os.close();
    progress.done();

    ProgressIndicator progress2("Converting to TIFF");
    
    // Create TIFF file
    if (!tifFile.empty()) {
        string options = "-p rgb -b 3";
        options += " -w " + std::to_string(width);
        options += " -l " + std::to_string(height);
        string command = opt.tools.raw2tiff + " " + options + " " + rawFile + " " + tifFile;

        if (system(command.c_str()) != 0) {
            throw Exception("Failed to execute " + command);
        }
    }

    // Create PNG file
    if (!pngFile.empty()) {

        string options = "";
        string command = opt.tools.convert + " " + options + " " + tifFile + " " + pngFile;

        if (system(command.c_str()) != 0) {
            throw Exception("Failed to execute " + command);
        }

        // Remove the temporary TIFF file
        fs::remove(tifFile);

    }

    // Remove the temporary RAW file
    fs::remove(rawFile);
    
    progress2.done();
    
    if (opt.verbose) {
        
        log::cout << log::vspace;
        log::cout << log::ralign("Input: ");
        log::cout << rawFile << log::endl;
        log::cout << log::ralign("Output: ");
        log::cout << tifFile << log::endl;
        log::cout << log::ralign("TIFF Converter: ");
        log::cout << opt.tools.raw2tiff << log::endl;
        log::cout << log::ralign("PNG Converter: ");
        log::cout << opt.tools.convert << log::endl;
    }
}

}
