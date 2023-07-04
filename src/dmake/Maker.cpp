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
#include "Application.h"
#include "Coord.h"
#include "IO.h"
#include "Logger.h"
#include "Options.h"
#include "ProgressIndicator.h"

namespace dd {

Maker::Maker(Application &app, Options &opt) : app(app), opt(opt)
{
    projectDir = opt.files.outputs.front();
}

void
Maker::generate()
{
    auto printReport = [&]() {

        if (!report.empty() && opt.flags.verbose) {

            log::cout << log::vspace;

            for (auto &it : report) {

                switch (it.second) {

                    case Action::CREATED:

                        log::cout << log::red << log::bold << log::ralign("Created: ");
                        log::cout << it.first << log::light << log::endl;
                        break;

                    case Action::SKIPPED:

                        log::cout << log::green << log::bold << log::ralign("Skipped: ");
                        log::cout << it.first << log::light << log::endl;
                        break;

                    case Action::MODIFIED:

                        log::cout << log::red << log::bold << log::ralign("Updated: ");
                        log::cout << it.first << log::light << log::endl;
                        break;
                }
            }
            log::cout << log::vspace;

            report = { };
        }
    };

    // Adjust some settings

    // The drill map resolution must at least be twice the image resolution
    opt.drillmap.width = std::min(opt.drillmap.width, 2 * opt.image.width);
    opt.drillmap.height = std::min(opt.drillmap.height, 2 * opt.image.height);

    generateProjectFile();
    printReport();

    generateIniFiles();
    printReport();

    generateMakefile();
    printReport();
}

void
Maker::generateProjectFile()
{
    ProgressIndicator progress("Generating zoomer ini file");

    auto temp = fs::temp_directory_path() / AssetManager::iniFile();

    // Open output stream
    std::ofstream os(temp);

    // Write header
    writeHeader(os);

    // Write sections
    writeLocationSection(os);
    writeMapSection(os);
    writeImageSection(os);
    writePaletteSection(os);
    writeTextureSection(os);
    writeLightingSection(os);
    writeVideoSection(os);

    copy(temp, projectDir / AssetManager::iniFile());
}

void
Maker::generateIniFiles()
{
    ProgressIndicator progress("Generating " + std::to_string(opt.video.keyframes) + " ini files");

    mpf_class zoom = 1.0;

    for (isize nr = 0; nr <= opt.video.keyframes; nr++, zoom *= 2.0) {

        app.readConfigFiles(nr);
        generateIniFile(nr, zoom);
    }
}

void
Maker::generateIniFile(isize nr, const mpf_class &zoom)
{
    // Overwrite zoom level
    std::stringstream ss; ss << zoom;
    opt.keys["location.zoom"] = ss.str();

    // Assemble path name
    auto temp = fs::temp_directory_path() / AssetManager::iniFile(nr);

    // Open output stream
    std::ofstream os(temp);
    os.precision(mpf_get_default_prec());

    // Write header
    writeHeader(os);

    // Write location section
    writeLocationSection(os);
    writeMapSection(os);
    writeImageSection(os);
    writePaletteSection(os);
    writeTextureSection(os);
    writeLightingSection(os);
    writePerturbationSection(os);
    writeApproximationSection(os);
    writeAreacheckSection(os);
    writePeriodcheckSection(os);
    writeAttractorcheckSection(os);

    copy(temp, projectDir / AssetManager::iniFile(nr));
}

void
Maker::writeLocationSection(std::ofstream &os)
{
    os << "[location]" << std::endl;
    os << "real = " << opt.keys["location.real"] << std::endl;
    os << "imag = " << opt.keys["location.imag"] << std::endl;
    os << "zoom = " << opt.keys["location.zoom"] << std::endl;
    os << "depth = " << opt.keys["location.depth"] << std::endl;
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
    os << std::endl;
}

void
Maker::writePaletteSection(std::ofstream &os)
{
    os << "[palette]" << std::endl;
    os << "image = " << opt.keys["palette.image"] << std::endl;
    os << "mode = " << opt.keys["palette.mode"] << std::endl;
    os << "scale = " << opt.keys["palette.scale"] << std::endl;
    os << "offset = " << opt.keys["palette.offset"] << std::endl;
    os << std::endl;
}

void
Maker::writeTextureSection(std::ofstream &os)
{
    os << "[texture]" << std::endl;
    os << "image = " << opt.keys["texture.image"] << std::endl;
    os << "opacity = " << opt.keys["texture.opacity"] << std::endl;
    os << "scale = " << opt.keys["texture.scale"] << std::endl;
    os << "offset = " << opt.keys["texture.offset"] << std::endl;
    os << std::endl;
}

void
Maker::writeLightingSection(std::ofstream &os)
{
    os << "[lighting]" << std::endl;
    os << "enable = " << opt.keys["lighting.enable"] << std::endl;
    os << "alpha = " << opt.keys["lighting.alpha"] << std::endl;
    os << "beta = " << opt.keys["lighting.beta"] << std::endl;
    os << std::endl;
}

void
Maker::writeGpuSection(std::ofstream &os)
{
    os << "[gpu]" << std::endl;
    os << "colorizer = " << opt.gpu.colorizer << std::endl;
    os << "illuminator = " << opt.gpu.illuminator << std::endl;
    os << "scaler = " << opt.gpu.scaler << std::endl;
    os << std::endl;
}

void
Maker::writeVideoSection(std::ofstream &os)
{
    os << "[video]" << std::endl;
    os << "framerate = " << opt.video.frameRate << std::endl;
    os << "keyframes = " << opt.video.keyframes << std::endl;
    os << "velocity = " << opt.video.velocity << std::endl;
    os << "bitrate = " << opt.video.bitrate << std::endl;
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
Maker::writeAreacheckSection(std::ofstream &os)
{
    os << "[areacheck]" << std::endl;
    os << "enable = " << opt.keys["areacheck.enable"] << std::endl;
    os << "color = " << opt.keys["areacheck.color"] << std::endl;
    os << std::endl;
}

void
Maker::writePeriodcheckSection(std::ofstream &os)
{
    os << "[attractorcheck]" << std::endl;
    os << "enable = " << opt.keys["attractorcheck.enable"] << std::endl;
    os << "tolerance = " << opt.keys["attractorcheck.tolerance"] << std::endl;
    os << "color = " << opt.keys["attractorcheck.color"] << std::endl;
    os << std::endl;
}

void
Maker::writeAttractorcheckSection(std::ofstream &os)
{
    os << "[periodcheck]" << std::endl;
    os << "enable = " << opt.keys["periodcheck.enable"] << std::endl;
    os << "tolerance = " << opt.keys["periodcheck.tolerance"] << std::endl;
    os << "color = " << opt.keys["periodcheck.color"] << std::endl;
    os << std::endl;
}

void
Maker::generateMakefile()
{
    ProgressIndicator progress("Generating Makefile");

    // Assemble path name
    auto temp = fs::temp_directory_path() / "Makefile";

    // Open output stream
    std::ofstream os(temp);

    // Write sections
    writeHeader(os);
    writeDefinitions(os);
    writeTargets(os);

    copy(temp, projectDir / "Makefile");
}

void
Maker::writeHeader(std::ofstream &os)
{
    os << "# Generated by DeepDrill " << Application::version() << std::endl;
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
    os << "MAPS       = $(patsubst %.ini,%.map,$(wildcard *_*.ini))" << std::endl;
    os << "VIDEO      = " << AssetManager::movFile().string() << std::endl;
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
    os << "%.map: %.ini" << std::endl;
    os << "\t" << "@$(DEEPDRILL) -b -v";
    os << " $*.ini -o $*.map -o $*_preview.jpg image.width=320 image.height=200 > $*.log" << std::endl;
    os << std::endl;

    // Write 'jpg' target
    os << "%.jpg: %.map" << std::endl;
    os << "\t" << "@$(DEEPDRILL) -v";
    os << " $*.ini $*.map -o $*.jpg" << std::endl;
    os << std::endl;

    // Write 'mov' target
    os << "$(VIDEO): $(IMAGES)" << std::endl;
    os << "\t" << "@$(DEEPZOOM) " << AssetManager::iniFile();
    os << " -o $(VIDEO)" << std::endl;
    os << std::endl;

    // Write 'clean' target
    os << "clean:" << std::endl;
    os << "\t" << "@rm *.mov *.map *.jpg *.log" << std::endl;
    os << std::endl;
}

void
Maker::copy(const fs::path &from, const fs::path &to)
{
    if (!fs::exists(to)) {

        report.push_back( { to, Action::CREATED } );
        fs::copy(from, to);

    } else if (!compareFiles(from, to)) {

        report.push_back( { to, Action::MODIFIED } );
        fs::copy(from, to, std::filesystem::copy_options::overwrite_existing);

    } else {

        report.push_back( { to, Action::SKIPPED } );
    }
}

}
