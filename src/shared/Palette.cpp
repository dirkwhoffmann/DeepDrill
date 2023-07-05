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

#include "Palette.h"
#include "Exception.h"
#include "IO.h"
#include "Options.h"
#include "DrillMap.h"

#include <tgmath.h>

namespace dd {

const sf::Image &
Palette::getImage()
{
    const auto &path = Options::palette.image;

    if (path == "") {

        constexpr isize width = 4096;

        // Compute default palette programmatically
        palette.create(width, 16); // TODO: Change 16 to 1

        /* Create default palette
         *
         * The formula is an adaption from https://www.shadertoy.com/view/tllSWj
         * It was used in an GLSL shader in the following form:
         *
         *     vec4 mapColor(float mcol) {
         *        return vec4(0.5 + 0.5*cos(2.7+mcol*30.0 + vec3(0.0,.6,1.0)),1.0);
         *     }
         */
        for (isize i = 0; i < width; i++) {

            double v = double(i) / width * 2 * 3.14159;
            auto rr = isize(255.0 * (0.5 + 0.5 * cos(v + 0.0 - 2.7)));
            auto gg = isize(255.0 * (0.5 + 0.5 * cos(v + 0.6 - 2.7)));
            auto bb = isize(255.0 * (0.5 + 0.5 * cos(v + 1.0 - 2.7)));
            palette.setPixel(int(i), 0, sf::Color(u8(rr), u8(gg), u8(bb)));
        }

    } else {

        if (!palette.loadFromFile(path)) {
            throw Exception("Failed to load palette image");
        }
    }

    return palette;
}

const sf::Image &
Palette::getTextureImage()
{
    const auto &path = Options::texture.image;

    if (path == "") {

        // Compute a transparent image
        texture.create(1, 1);
        texture.setPixel(0, 0, sf::Color(0, 0, 0, 0));

    } else {

        if (!texture.loadFromFile(path)) {
            throw Exception("Failed to load texture image");
        }
    }

    return texture;
}

}
