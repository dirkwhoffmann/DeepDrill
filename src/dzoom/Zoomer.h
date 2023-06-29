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

#pragma once

#include "config.h"
#include "Types.h"
#include "Chrono.h"
#include "Coord.h"
#include "Recorder.h"
#include "Animated.h"
#include "DrillMap.h"
#include "ColorMap.h"
#include "Colorizer.h"
#include "Filter.h"
#include "ProgressIndicator.h"

#include <SFML/Graphics.hpp>
#include <future>

namespace dd {

class Zoomer {

    // Configuration options
    struct Options &opt;

    // The application window
    sf::RenderWindow window;

    // Drill maps (read from map files)
    DrillMap drillMap[4] = { DrillMap(opt), DrillMap(opt), DrillMap(opt), DrillMap(opt) };

    // Currently loaded keyframe in each slot
    isize slot[4] = { -1, -1, -1, -1 };

    // Indicates whether a mapfile needs to be updated
    enum class MapState { Dirty, Loading, UpToDate };
    MapState mapState[4] = { MapState::Dirty, MapState::Dirty, MapState::Dirty, MapState::Dirty };

    // Synchronizers for the async map file loader
    std::future<bool> loadResult[4];

    // Colorizer for converting the drill maps into images
    Colorizer colorizer = Colorizer(opt);

    // The video recorder
    Recorder recorder = Recorder(opt);

    // Indicates if the application runs in record mode or preview mode
    bool recordMode;

    // The currently processed keyframe and inbetween
    isize keyframe = 0;
    isize frame = 0;

    // Animation parameters
    Animated zoom;

    // Stop watches
    Clock updateClock;
    Clock renderClock;
    Clock recordClock;
    Time updateTime;
    // Time renderTime;
    // Time recordTime;

    // Progress indicator
    ProgressIndicator progress;

public:

    // Constructors
    Zoomer(Options &opt);
    ~Zoomer();
    
    // Initializers
    void init();

    // Main entry point
    void launch();

private:

    // The main loop
    void mainLoop();

    // Called inside the main loop
    void report();
    void animate();
    void update();
    void draw();
    void record();

    // Returns the map slot for a given keyframe
    isize slotNr(isize nr) { return (nr + 4) % 4; }

    // Loads a new map file from disk
    std::future<bool> loadMapFileAsync(isize nr);
    bool loadMapFile(isize nr);
};

}
