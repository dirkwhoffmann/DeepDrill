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
#include "Coord.h"
#include "Recorder.h"
#include "Animated.h"
#include "DrillMap.h"
#include "ColorMap.h"
#include "Filter.h"

#include <SFML/Graphics.hpp>

namespace dd {

class Zoomer {

    // Configuration options
    struct Options &opt;

    // The application window
    sf::RenderWindow window;

    // Drill maps (read from map files)
    DrillMap drillMap = DrillMap(opt);
    DrillMap drillMap2 = DrillMap(opt);

    // Textures storing the current keyframe (read from file)
    sf::Texture source1;
    sf::Texture normal1;

    // Textures storing the next keyframe (read from file)
    sf::Texture source2;
    sf::Texture normal2;

    // GPU filters
    Filter illuminator1 = Filter(opt);
    Filter illuminator2 = Filter(opt);
    Filter downscaler = Filter(opt);
    
    // The video recorder
    Recorder recorder = Recorder(opt);

    // Indicates if we run in record mode or preview mode
    bool recordMode;

    // The currently processed keyframe and inbetween
    isize keyframe = 0;
    isize frame = 0;

    // Animation parameters
    Animated zoom;

public:

    // Constructor
    Zoomer(Options &opt);

    // Initializers
    void init();
    void initTexture(sf::Texture &tex, sf::Vector2u size);

    // Main entry point
    void launch();

private:

    // Called inside the main loop
    void update();
    void draw();

    // Loads a new image file from disk
    void updateTextures(isize nr);

    // Loads a new location from disk
    void updateLocation(isize nr);
};

}
