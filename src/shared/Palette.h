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

#include <SFML/Graphics.hpp>

namespace dd {

class Palette {

    // The palette image
    sf::Image palette;

    // The texture image
    sf::Image texture;

public:

    const sf::Image &getImage();
    const sf::Image &getTextureImage();
};

}
