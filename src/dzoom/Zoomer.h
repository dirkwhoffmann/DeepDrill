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

#include <SFML/Graphics.hpp>

namespace dd {

class Zoomer {

    // Configuration options
    struct Options &opt;

    // The application window
    sf::RenderWindow window;
    
    // The source texture (read from image file)
    sf::Texture source;
    sf::RectangleShape sourceRect;

    // Experimental
    sf::Texture source2;
    sf::RectangleShape sourceRect2;

    // The downscaled source texture
    sf::RenderTexture scaled;
    sf::RectangleShape scaledRect;

    // The render target
    sf::RenderTexture target;
    sf::RectangleShape targetRect;

    // Compute kernels
    sf::Shader scaler;
    sf::Shader illuminator;

    // The video recorder
    Recorder recorder = Recorder(opt);

    // Indicates if we run in record mode or preview mode
    bool recordMode;

    // Animation parameters
    Animated w, h;

public:

    // Constructor
    Zoomer(Options &opt);

    // Initializers
    void init();

    // Main entry point
    void launch();

private:

    // Called inside the main loop
    void update(isize keyframe, isize frame);
    void draw(isize keyframe, isize frame);

    // Sets up shader uniforms
    void setupScalerUniforms(isize keyframe, isize frame);
    void setupIlluminatorUniforms(isize keyframe, isize frame);

    // Loads a new image file from disk
    void updateTexture(isize nr);

    // Loads a new location from disk
    void updateLocation(isize nr);
};

}
