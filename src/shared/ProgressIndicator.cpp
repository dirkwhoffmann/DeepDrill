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

#include "ProgressIndicator.h"
#include "AssetManager.h"
#include "IO.h"
#include "Logger.h"
#include "Options.h"

namespace dd {

ProgressIndicator::ProgressIndicator(const string &description, isize max)
{
    init(description, max);
}

ProgressIndicator::~ProgressIndicator()
{
    done();
}

void
ProgressIndicator::init(const string &description, isize max)
{
    this->description = description;
    progress = 0;
    progressMax = max;
    
    dots = 0;
    dotsMax = 33;

    log::cout << log::ralign(description + ": ");
    log::cout << log::flush;
    clock.restart();
}

void
ProgressIndicator::step(isize delta)
{
    if (clock.isPaused()) return;

    progress += delta;

    isize newDots = dotsMax * progress / progressMax;
    for (; dots < newDots; dots++) { log::cout << "."; }
    log::cout << log::flush;
}

void
ProgressIndicator::done(const string &info)
{
    if (clock.isPaused()) return;

    auto elapsed = clock.stop();
        
    for (; dots < dotsMax; dots++) { log::cout << "."; } log::cout << " ";
    log::cout << elapsed;
    
    if (info != "") log::cout << " (" << info << ")";
    log::cout << log::endl;
}

BatchProgressIndicator::BatchProgressIndicator(const Options &opt, const string &msg, const fs::path &path)
{
    if (opt.flags.batch) {

        this->msg = msg;
        this->path = path;

        std::stringstream ss;
        Logger logger(ss);

        prefix(logger);
        logger << log::yellow << msg << " " << path << log::normal;
        logger << " ..." << log::endl;

        std::cerr << ss.str();
        clock.restart();
    }
}

BatchProgressIndicator::~BatchProgressIndicator()
{
    if (msg != "") {

        std::stringstream ss;
        Logger logger(ss);

        prefix(logger);
        logger << log::green << "Created " << path;
        logger << log::normal << " (" << clock.stop() << ")" << log::endl;

        std::cerr << ss.str();
    }
}

void
BatchProgressIndicator::prefix(Logger &logger)
{
    isize total = 0;
    isize done = 0;

    for (const auto &it : fs::directory_iterator(fs::current_path())) {

        auto file = it.path();

        if (file.extension() != ".ini") continue;
        if (file.filename() == AssetManager::iniFile()) continue;

        auto iniFile = file;
        auto mapFile = iniFile.replace_extension(".map");
        total += 1;
        done += isOlderThan(iniFile, mapFile);
        /*
        if (fs::exists(mapFile)) {

            auto iniDate = fs::last_write_time(iniFile).time_since_epoch();
            auto mapDate = fs::last_write_time(mapFile).time_since_epoch();
            done += isize(mapDate >= iniDate);
        }
        */
    }

    if (total) {
        logger << log::blue << "[" << isize(100.0 * done / total) << "%] ";
    } else {
        logger << log::blue << "[-] ";
    }
}

}
