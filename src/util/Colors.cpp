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

#include "Colors.h"

namespace dd {

//
// RgbColor
//

RgbColor::RgbColor(const GpuColor &c)
{
    r = (c.rawValue & 0xFF) / 255.0;
    g = ((c.rawValue >> 8) & 0xFF) / 255.0;
    b = ((c.rawValue >> 16) & 0xFF) / 255.0;
}

RgbColor::RgbColor(const YuvColor &c)
{
    r = c.y + 1.140 * c.v;
    g = c.y - 0.395 * c.u - 0.581 * c.v;
    b = c.y + 2.032 * c.u;
}

RgbColor
RgbColor::mix(const RgbColor &other, double weight) const
{
    assert(other.r <= 1.0);
    assert(other.g <= 1.0);
    assert(other.b <= 1.0);

    double newR = r + (other.r - r) * weight;
    double newG = g + (other.g - g) * weight;
    double newB = b + (other.b - b) * weight;

    return RgbColor(newR, newG, newB);
}

const RgbColor RgbColor::black(0.0, 0.0, 0.0);
const RgbColor RgbColor::white(1.0, 1.0, 1.0);
const RgbColor RgbColor::red(1.0, 0.0, 0.0);
const RgbColor RgbColor::green(0.0, 1.0, 0.0);
const RgbColor RgbColor::blue(0.0, 0.0, 1.0);
const RgbColor RgbColor::yellow(1.0, 1.0, 0.0);
const RgbColor RgbColor::magenta(1.0, 0.0, 1.0);
const RgbColor RgbColor::cyan(0.0, 1.0, 1.0);


//
// YuvColor
//

YuvColor::YuvColor(const RgbColor &c)
{
    y =  0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
    u = -0.147 * c.r - 0.289 * c.g + 0.436 * c.b;
    v =  0.615 * c.r - 0.515 * c.g - 0.100 * c.b;
}

const YuvColor YuvColor::black(RgbColor::black);
const YuvColor YuvColor::white(RgbColor::white);
const YuvColor YuvColor::red(RgbColor::red);
const YuvColor YuvColor::green(RgbColor::green);
const YuvColor YuvColor::blue(RgbColor::blue);
const YuvColor YuvColor::yellow(RgbColor::yellow);
const YuvColor YuvColor::magenta(RgbColor::magenta);
const YuvColor YuvColor::cyan(RgbColor::cyan);

YuvColor
YuvColor::mix(const YuvColor &other, double weight) const
{
    RgbColor mixedColor = RgbColor(*this).mix(other, weight);
    return GpuColor(mixedColor);
}


//
// GpuColor
//

GpuColor::GpuColor(const RgbColor &c)
{
    auto a = 255;
    auto r = u8(c.r * 255);
    auto g = u8(c.g * 255);
    auto b = u8(c.b * 255);

    rawValue = u32(a << 24 | b << 16 | g << 8 | r);
}

GpuColor::GpuColor(u8 r, u8 g, u8 b)
{
    auto a = 255;
    rawValue = u32(a << 24 | b << 16 | g << 8 | r);
}

const GpuColor GpuColor::black(RgbColor::black);
const GpuColor GpuColor::white(RgbColor::white);
const GpuColor GpuColor::red(RgbColor::red);
const GpuColor GpuColor::green(RgbColor::green);
const GpuColor GpuColor::blue(RgbColor::blue);
const GpuColor GpuColor::yellow(RgbColor::yellow);
const GpuColor GpuColor::magenta(RgbColor::magenta);
const GpuColor GpuColor::cyan(RgbColor::cyan);

GpuColor
GpuColor::mix(const RgbColor &other, double weight) const
{
    RgbColor mixedColor = RgbColor(*this).mix(other, weight);
    return GpuColor(mixedColor);
}

}
