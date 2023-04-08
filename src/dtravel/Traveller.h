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

#include "commons.h"
#include "Recorder.h"
#include <SFML/Graphics.hpp>

namespace dd {

class Traveller {

    // Configuration options
    const struct Options &opt;

    // The application window
    sf::RenderWindow window;
    
    // The render source
    sf::Texture source;
    sf::RectangleShape sourceRect;

    // The render target
    sf::RenderTexture target;
    sf::RectangleShape targetRect;

    // The compute kernel
    sf::Shader shader;
    
    // The video recorder
    Recorder recorder = Recorder(opt);

public:

    // Constructor
    Traveller(Options &opt);

    // Initializer
    void init();

    // Main entry point
    void launch();

private:

    // Loads a new texture from disk
    void updateTexture(isize nr); 
};

}
