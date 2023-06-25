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

    // Configuration options
    const struct Options &opt;

    // Color values
    std::vector <double> r;
    std::vector <double> g;
    std::vector <double> b;

    // Overlay image
    sf::Image texture;

public:

    Palette(const Options &options);

    void loadPaletteImage(const fs::path &path);
    void loadTextureImage(const fs::path &path);

    double overlayOpacity() const;
    u32 interpolateABGR(struct MapEntry &entry) const;
    u32 readTextureImage(struct MapEntry &entry) const;
};

}
