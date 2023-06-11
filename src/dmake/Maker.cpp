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
    project = stripSuffix(stripPath(opt.files.inputs.front()));
    projectDir = opt.files.outputs.front();
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

    // Adjust some settings

    // The drill map resolution must at least be twice the image resolution
    opt.drillmap.width = std::min(opt.drillmap.width, 2 * opt.image.width);
    opt.drillmap.height = std::min(opt.drillmap.height, 2 * opt.image.height);

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

    auto path = projectDir / (project + ".prj");

    // Open output stream
    std::ofstream os(path);

    // Write header
    writeHeader(os);

    // Write sections
    writeLocationSection(os);
    writeMapSection(os);
    writeImageSection(os);
    writeColorsSection(os);
    writeVideoSection(os);
}

void
Maker::generateLocationFiles(vector <string> &skipped)
{
    ProgressIndicator progress("Generating " + std::to_string(opt.video.keyframes) + " location files");

    for (isize nr = 0; nr <= opt.video.keyframes; nr++) {

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

        // Write location section
        writeLocationSection(os);
        os << "zoom = " << std::to_string(zoom) << std::endl;
        os << "depth = " << std::to_string(depth) << std::endl;
        os << std::endl;
    }
}

void
Maker::generateProfile(vector <string> &skipped)
{
    ProgressIndicator progress("Generating profile");

    // Assemble path name
    auto name = project + ".prf";
    auto path = projectDir / name;

    // Only proceed if file does not exist yet
    if (fileExists(path)) { skipped.push_back(name); return; }

    // Open output stream
    std::ofstream os(path);

    // Write sections
    writeHeader(os);
    writeMapSection(os);
    writePreviewImageSection(os);
    writeColorsSection(os);
    writePerturbationSection(os);
    writeApproximationSection(os);
    writeDebugSection(os);
}

void
Maker::writeLocationSection(std::ofstream &os)
{
    os << "[location]" << std::endl;
    os << "real = " << opt.keys["location.real"] << std::endl;
    os << "imag = " << opt.keys["location.imag"] << std::endl;
    os << std::endl;
}

void
Maker::writeMapSection(std::ofstream &os)
{
    // The DrillMap resolution must be at leas twice the image resolution
    auto width = std::max(opt.drillmap.width, opt.image.width * 2);
    auto height = std::max(opt.drillmap.height, opt.image.height * 2);

    os << "[map]" << std::endl;
    os << "width = " << width << std::endl;
    os << "height = " << height << std::endl;
    os << std::endl;
}

void
Maker::writeImageSection(std::ofstream &os)
{
    os << "[image]" << std::endl;
    os << "width = " << opt.image.width << std::endl;
    os << "height = " << opt.image.height << std::endl;
    os << "illuminator = " << opt.image.illuminator << std::endl;
    os << "scaler = " << opt.image.scaler << std::endl;
    os << std::endl;
}

void
Maker::writePreviewImageSection(std::ofstream &os)
{
    os << "[image]" << std::endl;
    os << "width = 320    # Preview image width" << std::endl;
    os << "height = 200   # Preview image height" << std::endl;
    os << "illuminator = " << opt.image.illuminator << std::endl;
    os << "scaler = " << opt.image.scaler << std::endl;
    os << std::endl;
}

void
Maker::writeColorsSection(std::ofstream &os)
{
    os << "[colors]" << std::endl;
    os << "mode = " << opt.keys["colors.mode"] << std::endl;
    os << "palette = " << opt.keys["colors.palette"] << std::endl;
    os << "scale = " << opt.keys["colors.scale"] << std::endl;
    os << "alpha = " << opt.keys["colors.alpha"] << std::endl;
    os << "beta = " << opt.keys["colors.beta"] << std::endl;
    os << std::endl;
}

void
Maker::writeVideoSection(std::ofstream &os)
{
    os << "[video]" << std::endl;
    os << "framerate = " << opt.video.frameRate << std::endl;
    os << "keyframes = " << opt.video.keyframes << std::endl;
    os << "inbetweens = " << opt.video.inbetweens << std::endl;
    os << "bitrate = " << opt.video.bitrate << std::endl;
    os << "scaler = " << opt.video.scaler << std::endl;
    os << std::endl;
}

void
Maker::writePerturbationSection(std::ofstream &os)
{
    os << "[perturbation]" << std::endl;
    os << "enable = " << opt.keys["perturbation.enable"] << std::endl;
    os << "tolerance = " << opt.keys["perturbation.tolerance"] << std::endl;
    os << "rounds = " << opt.keys["perturbation.rounds"] << std::endl;
    os << std::endl;
}

void
Maker::writeApproximationSection(std::ofstream &os)
{
    os << "[approximation]" << std::endl;
    os << "enable = " << opt.keys["approximation.enable"] << std::endl;
    os << "coefficients = " << opt.keys["approximation.coefficients"] << std::endl;
    os << "tolerance = " << opt.keys["approximation.tolerance"] << std::endl;
    os << std::endl;
}

void
Maker::writeDebugSection(std::ofstream &os)
{
    os << "[debug]" << std::endl;
    os << "glitches = " << opt.keys["debug.glitches"] << std::endl;
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
    os << "DEEPZOOM   = " << (path / "deepzoom").string() << std::endl;
    os << "MAPS       = $(patsubst %.loc,%.map,$(wildcard *_*.loc))" << std::endl;
    os << "VIDEO      = " << project << ".mov" << std::endl;
    os << "MAPFLAGS   = -b -v" << std::endl;
    os << std::endl;
}

void
Maker::writeTargets(std::ofstream &os)
{
    // Declare phony targets
    os << ".PHONY: all maps clean" << std::endl;
    os << std::endl;

    // Write the 'all' target and some sub targets
    os << "all: maps" << std::endl;
    os << std::endl;

    os << "maps: $(MAPS)" << std::endl;
    os << std::endl;

    // Write 'map' target
    os << "%.map: %.loc" << std::endl;
    os << "\t" << "@$(DEEPDRILL) $(MAPFLAGS) -p " << project << ".prf";
    os << " -o $*.map -o $*.jpg $*.loc > $*.log" << std::endl;
    os << std::endl;

    // Write 'mov' target
    os << "$(VIDEO): $(IMAGES)" << std::endl;
    os << "\t" << "@$(DEEPZOOM) $(MOVFLAGS) " << project << ".prj";
    os << " -o $(VIDEO)" << std::endl;
    os << std::endl;

    // Write 'clean' target
    os << "clean:" << std::endl;
    os << "\t" << "rm *.mov *.map *.jpg *.log" << std::endl;
    os << std::endl;
}

}
