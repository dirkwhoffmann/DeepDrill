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

#include "Maker.h"
#include "Coord.h"
#include "IO.h"
#include "Logger.h"
#include "Options.h"
#include "ProgressIndicator.h"

namespace dd {

Maker::Maker(Options &o) : opt(o)
{
    project = stripSuffix(stripPath(opt.files.input));
    projectDir = opt.files.output;
}

void
Maker::generate()
{
    vector <string> skipped;

    auto reportSkippedFiles = [&]() {

        if (!skipped.empty()) {

            log::cout << log::vspace;
            for (auto &it : skipped) {

                log::cout << log::purple << log::bold << log::ralign("Skipping file: ") << log::light;
                log::cout << log::normal << it << " already exists" << log::endl;
            }
            log::cout << log::vspace;
            skipped = { };
        }
    };

    generateProjectFile(skipped);
    reportSkippedFiles();

    generateLocationFiles(skipped);
    reportSkippedFiles();

    generateProfile(skipped);
    reportSkippedFiles();

    generateMakefile(skipped);
    reportSkippedFiles();
}

void
Maker::generateProjectFile(vector <string> &skipped)
{
    ProgressIndicator progress("Generating project file");

    auto &keys = opt.keys;
    auto path = projectDir / (project + ".prj");

    // Open output stream
    std::ofstream os(path);

    // Write header
    writeHeader(os);

    // Write location section
    os << "[location]" << std::endl;
    os << "real = " << keys["location.real"] << std::endl;
    os << "imag = " << keys["location.imag"] << std::endl;
    os << std::endl;

    // Write image section
    os << "[image]" << std::endl;
    os << "width = " << opt.image.width << std::endl;
    os << "height = " << opt.image.height << std::endl;
    os << std::endl;

    // Write video section
    os << "[video]" << std::endl;
    os << "framerate = " << opt.video.frameRate << std::endl;
    os << "width = " << opt.video.width << std::endl;
    os << "height = " << opt.video.height << std::endl;
    os << "keyframes = " << opt.video.keyframes << std::endl;
    os << "inbetweens = " << opt.video.inbetweens << std::endl;
    os << "bitrate = " << opt.video.bitrate << std::endl;
    os << "scaler = " << opt.video.scaler << std::endl;
    os << "merger = " << opt.video.merger << std::endl;
    os << std::endl;
}

void
Maker::generateLocationFiles(vector <string> &skipped)
{
    ProgressIndicator progress("Generating " + std::to_string(opt.video.keyframes) + " location files");

    // Start in the middle of the Mandelbrot set by shifting the center
    auto shift = PrecisionComplex(); // PrecisionComplex(-opt.location.real, -opt.location.imag);
    for (isize nr = 0; nr < opt.video.keyframes; nr++) {

        double zoom = exp2(nr);

        // Assemble path name
        auto name = (project + "_" + std::to_string(nr) + ".loc");
        auto path = projectDir / name;

        // Only proceed if file does not exist yet
        if (fileExists(path)) { skipped.push_back(name); continue; }

        // Open output stream
        std::ofstream os(path);
        os.precision(mpf_get_default_prec());

        // Write header
        writeHeader(os);

        // Limit depth
        isize maxDepth = 1000 * (nr + 1);
        isize depth = std::min(maxDepth, opt.location.depth);

        // Shift the center coordinate
        auto center = PrecisionComplex(opt.location.real, opt.location.imag) + shift;
        shift *= 0.5 * 0.9;

        // Write location section
        os << "[location]" << std::endl;
        os << "real = " << center.re << std::endl;
        os << "imag = " << center.im << std::endl;
        os << "zoom = " << std::to_string(zoom) << std::endl;
        os << "depth = " << std::to_string(depth) << std::endl;
        os << std::endl;
    }
}

void
Maker::generateProfile(vector <string> &skipped)
{
    ProgressIndicator progress("Generating profile");

    auto &keys = opt.keys;

    // Assemble path name
    auto name = project + ".prf";
    auto path = projectDir / name;

    // Only proceed if file does not exist yet
    if (fileExists(path)) { skipped.push_back(name); return; }

    // Open output stream
    std::ofstream os(path);
    
    // Write header
    writeHeader(os);

    // Write image section
    os << "[image]" << std::endl;
    os << "width = " << keys["image.width"] << std::endl;
    os << "height = " << keys["image.height"] << std::endl;
    os << std::endl;

    // Write perturbation section
    os << "[perturbation]" << std::endl;
    os << "tolerance = " << keys["perturbation.tolerance"] << std::endl;
    os << "rounds = " << keys["perturbation.rounds"] << std::endl;
    os << std::endl;

    // Write series approximation section
    os << "[approximation]" << std::endl;
    os << "coefficients = " << keys["approximation.coefficients"] << std::endl;
    os << "tolerance = " << keys["approximation.tolerance"] << std::endl;
    os << std::endl;

    // Write palette section
    os << "[palette]" << std::endl;
    os << "colors = " << keys["palette.colors"] << std::endl;
    os << "scale = " << keys["palette.scale"] << std::endl;
    os << std::endl;
}

void
Maker::generateMakefile(vector <string> &skipped)
{
    ProgressIndicator progress("Generating Makefile");

    // Open output stream
    std::ofstream os(projectDir / "Makefile");

    // Write sections
    writeHeader(os);
    writeDefinitions(os);
    writeTargets(os);
}

void
Maker::writeHeader(std::ofstream &os)
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    os << "# Generated by DeepDrill " << Options::version();
    os << " on " << std::ctime(&time);
    os << "# " << std::endl;
    os << "# Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de" << std::endl;
    os << "# Licensed under the GNU General Public License v3" << std::endl;
    os << std::endl;
}

void
Maker::writeDefinitions(std::ofstream &os)
{
    auto path = opt.files.exec.parent_path();

    os << "DEEPDRILL  = " << (path / "deepdrill").string() << std::endl;
    os << "DEEPFLIGHT = " << (path / "deepflight").string() << std::endl;
    os << "MAPS       = $(patsubst %.loc,%.map,$(wildcard *_*.loc))" << std::endl;
    os << "IMAGES     = $(patsubst %.loc,%.png,$(wildcard *_*.loc))" << std::endl;
    os << "VIDEO      = " << project << ".mov" << std::endl;
    os << "NUM_IMAGES = $(words $(IMAGES))" << std::endl;
    os << "MAPFLAGS   = -b" << std::endl;
    os << "PNGFLAGS   = -b" << std::endl;
    os << std::endl;
}

void
Maker::writeTargets(std::ofstream &os)
{
    // Declare phony targets
    os << ".PHONY: all maps images clean" << std::endl;
    os << std::endl;

    // Write the 'all' target and some sub targets
    os << "all: images" << std::endl;
    os << std::endl;

    os << "maps: $(MAPS)" << std::endl;
    os << std::endl;

    os << "images: $(IMAGES) maps" << std::endl;
    os << std::endl;

    // Write 'map' target
    os << "%.map: %.loc" << std::endl;
    os << "\t" << "@$(DEEPDRILL) $(MAPFLAGS) -p " << project << ".prf";
    os << " -o $*.map $*.loc > $*.log" << std::endl;
    os << std::endl;

    // Write 'png' target
    os << "%.png: %.map" << std::endl;
    os << "\t" << "@$(DEEPDRILL) $(PNGFLAGS) -p " << project << ".prf";
    os << " -o $*.png $*.map > /dev/null" << std::endl;
    os << std::endl;

    // Write 'mov' target
    os << "$(VIDEO): $(IMAGES)" << std::endl;
    os << "\t" << "@$(DEEPFLIGHT) $(MOVFLAGS) " << project << ".prj";
    os << " -o $(VIDEO)";
    os << std::endl;

    // Write 'clean' target
    os << "clean:" << std::endl;
    os << "\t" << "rm *.mov *.map *.png *.log" << std::endl;
    os << std::endl;
}

}
