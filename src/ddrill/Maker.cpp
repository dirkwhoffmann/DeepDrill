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
#include "Maker.h"
#include "Coord.h"
#include "Logger.h"
#include "Options.h"
#include "ProgressIndicator.h"

namespace dd {

Maker::Maker(Options &o) : opt(o)
{
    project = stripSuffix(stripPath(opt.input));
    projectDir = std::filesystem::path(opt.output);
}

void
Maker::generate()
{
    {
        ProgressIndicator progress("Generating project file");
        generateProjectFile();
    }
    {
        ProgressIndicator progress("Generating location files");
        generateLocationFiles();
    }
    {
        ProgressIndicator progress("Generating profile");
        generateProfile();
    }
    {
        ProgressIndicator progress("Generating Makefile");
        generateMakefile();
    }
}

void
Maker::generateProjectFile()
{
    auto &keys = opt.keys;

    // Open output stream
    std::ofstream os(projectDir / (project + ".prj"));

    // Write header
    writeHeader(os);

    // Write location section
    os << "[location]" << std::endl;
    os << "real = " << keys["location.real"] << std::endl;
    os << "imag = " << keys["location.imag"] << std::endl;
    os << std::endl;

    // Write image section
    os << "[image]" << std::endl;
    os << "width = " << keys["image.width"] << std::endl;
    os << "height = " << keys["image.height"] << std::endl;
    os << std::endl;

    // Write video section
    os << "[video]" << std::endl;
    os << "framerate = " << keys["video.framerate"] << std::endl;
    os << "width = " << keys["video.width"] << std::endl;
    os << "height = " << keys["video.height"] << std::endl;
    os << "keyframes = " << keys["video.keyframes"] << std::endl;
    os << "inbetweens = " << keys["video.inbetweens"] << std::endl;
    os << "duration = " << keys["video.duration"] << std::endl;
    os << "bitrate = " << keys["video.bitrate"] << std::endl;
    os << "scaler = " << keys["video.scaler"] << std::endl;
    os << std::endl;
}

void
Maker::generateLocationFiles()
{
    shift = PrecisionComplex(opt.location.dreal, opt.location.dimag);

    for (isize nr = 0; nr < opt.video.keyframes; nr++) {

        auto &keys = opt.keys;
        double zoom = exp2(nr);

        // Open output stream
        std::ofstream os(projectDir / (project + "_" + std::to_string(nr) + ".loc"));

        // Write header
        writeHeader(os);

        // Limit depth
        isize maxDepth = 1000 * (nr + 1);
        isize depth = std::min(maxDepth, opt.location.depth);

        // Write location section
        os << "[location]" << std::endl;
        os << "real = " << keys["location.real"] << std::endl;
        os << "imag = " << keys["location.imag"] << std::endl;
        os << "dreal = " << shift.re << std::endl;
        os << "dimag = " << shift.im << std::endl;
        os << "zoom = " << std::to_string(zoom) << std::endl;
        os << "depth = " << std::to_string(depth) << std::endl;
        os << std::endl;

        // Write image section
        os << "[image]" << std::endl;
        os << "width = " << keys["image.width"] << std::endl;
        os << "height = " << keys["image.height"] << std::endl;
        os << std::endl;

        // Compute center shift
        mpf_class pixelDelta = mpf_class(4.0) / zoom / opt.image.height;
        auto oldShift = shift;
        shift *= 0.5 * 0.9;
        auto delta = (shift - oldShift) / pixelDelta;

        // Write animation section
        os << "[animation]" << std::endl;
        os << "dx = " << isize(std::round(delta.re.get_d())) << std::endl;
        os << "dy = " << isize(std::round(delta.im.get_d())) << std::endl;
    }
}

void
Maker::generateProfile()
{
    auto &keys = opt.keys;
    
    // Open output stream
    std::ofstream os(projectDir / (project + ".prf"));
    
    // Write header
    writeHeader(os);

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
    os << "values = " << keys["palette.values"] << std::endl;
    os << std::endl;
}

void
Maker::generateMakefile()
{
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
    
    os << "# Generated by DeepDrill " << VER_MAJOR << "." << VER_MINOR;
    os << " on " << std::ctime(&time);
    os << std::endl;
}

void
Maker::writeDefinitions(std::ofstream &os)
{
    os << "DEEPDRILL  = " << opt.exec << std::endl;
    os << "MAPS       = $(patsubst %.loc,%.map,$(wildcard *_*.loc))" << std::endl;
    os << "IMAGES     = $(patsubst %.loc,%.png,$(wildcard *_*.loc))" << std::endl;
    os << "NUM_IMAGES = $(words $(IMAGES))" << std::endl;
    os << std::endl;
}

void
Maker::writeTargets(std::ofstream &os)
{
    // Declare phony targets
    os << ".PHONY: all init clean" << std::endl;
    os << std::endl;

    // Write 'all' target
    os << "all: init $(IMAGES) $(MAPS)" << std::endl;
    os << "\t" << "@echo \"\"" << std::endl;
    os << std::endl;

    // Write 'init' target
    os << "init:" << std::endl;
    os << "\t" << "@echo \"DeepDrill Batch Processor " << VER_MAJOR << "." << VER_MINOR << "\"" << std::endl;
    os << "\t" << "@echo \"\"" << std::endl;
    os << "\t" << "@echo \"Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de\"" << std::endl;
    os << "\t" << "@echo \"Licensed under the GNU General Public License v3\"" << std::endl;
    os << "\t" << "@echo \"\"" << std::endl;
    os << "\t" << "@echo \"Generating $(NUM_IMAGES) images...\"" << std::endl;
    os << std::endl;

    // Write 'map' and 'png' targets
    os << "%.png: %.map" << std::endl;
    os << "\t" << "@$(DEEPDRILL) -b -p " << project << ".prf -o $*.png $*.map" << std::endl;
    os << std::endl;
    os << "%.map: %.loc" << std::endl;
    os << "\t" << "@$(DEEPDRILL) -b -p " << project << ".prf -o $*.map $*.loc" << std::endl;
    os << std::endl;

    // Write 'clean' target
    os << "clean:" << std::endl;
    os << "\t" << "rm *.map *.png" << std::endl;
    os << std::endl;
}

void
Maker::writeTarget(std::ofstream &os, isize nr)
{
    auto name = project + "_" + std::to_string(nr);

    os << name << ".png: " << name << ".map" << std::endl;
    os << "\t";
    os << "$(DEEPDRILL)";
    if (opt.verbose) os << " -v";
    os << " -p " << project << ".prf";
    os << " -o " << name << ".png";
    os << " " << name << ".map " << std::endl;
    os << std::endl;

    os << name << ".map: " << name << ".loc" << std::endl;
    os << "\t";
    os << "$(DEEPDRILL)";
    if (opt.verbose) os << " -v";
    os << " -p " << project << ".prf";
    os << " -o " << name << ".map";
    os << " " << name << ".loc " << std::endl;
    os << std::endl;
}

}
