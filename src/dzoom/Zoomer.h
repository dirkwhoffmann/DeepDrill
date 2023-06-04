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

#include <SFML/Graphics.hpp>

namespace dd {

class Zoomer {

    // Configuration options
    struct Options &opt;

    // The application window
    sf::RenderWindow window;

    // Current drill map (read from map file)
    DrillMap drillMap = DrillMap(opt);
    DrillMap drillMap2 = DrillMap(opt);

    // A colorizer for converting drill maps into images
    ColorMap colorizer = ColorMap(opt);

    // Texture storing the current keyframe (read from image file)
    sf::Texture source;
    sf::Texture normal;
    sf::RectangleShape sourceRect;

    // Texture storing the next keyframe (read from image file)
    sf::Texture source2;
    sf::Texture normal2;
    sf::RectangleShape sourceRect2;

    // Texture storing the illuminated current keyframe (computed)
    sf::RenderTexture illuminated;
    sf::RectangleShape illuminatedRect;

    // Texture storing the illuminated next keyframe (computed)
    sf::RenderTexture illuminated2;
    sf::RectangleShape illuminatedRect2;

    // The downscaled source texture (computed)
    sf::RenderTexture scaled;
    sf::RectangleShape scaledRect;

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
    void initTexture(sf::Texture &tex, sf::Vector2u size);
    void initTexture(sf::Texture &tex, sf::RectangleShape &rect, sf::Vector2u size);
    void initRenderTexture(sf::RenderTexture &tex, sf::Vector2u size);
    void initRenderTexture(sf::RenderTexture &tex, sf::RectangleShape &rect, sf::Vector2u size);
    void initShader(sf::Shader &shader, const string &name);

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
    void updateTextures(isize nr);

    // Loads a new location from disk
    void updateLocation(isize nr);
};

}
