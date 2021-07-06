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

namespace dd {

Palette::Palette(const Options &options) : opt(options)
{
    init(opt.palette);    
}

void
Palette::init(string values)
{
    std::stringstream stream(values);
    u32 value;

    while(stream >> value) {
        colors.push_back(value);
    }
}

u32
Palette::colorize(isize depth) const
{
    return colors[depth % colors.size()];
}

}
