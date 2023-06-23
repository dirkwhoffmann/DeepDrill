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

namespace dd {

struct RgbColor;
struct YuvColor;
struct GpuColor;

struct RgbColor {

    double r;
    double g;
    double b;

    RgbColor() : r(0), g(0), b(0) {}
    RgbColor(double r, double g, double b) : r(r), g(g), b(b) {}
    RgbColor(u8 r, u8 g, u8 b) : r(r / 255.0), g(g / 255.0), b(b / 255.0) {}
    RgbColor(u32 abgr) : RgbColor(u8(abgr), u8(abgr >> 8), u8(abgr >> 16)) {}
    RgbColor(const YuvColor &c);
    RgbColor(const GpuColor &c);

    static const RgbColor black;
    static const RgbColor white;
    static const RgbColor red;
    static const RgbColor green;
    static const RgbColor blue;
    static const RgbColor yellow;
    static const RgbColor magenta;
    static const RgbColor cyan;

    bool operator==(const RgbColor &rhs) const {
        return r == rhs.r && g == rhs.g && b == rhs.b;
    }

    RgbColor mix(const RgbColor &other, double weight) const;
    RgbColor tint(double weight) const { return mix(white, weight); }
    RgbColor shade(double weight) const { return mix(black, weight); }
};

struct YuvColor {

    double y;
    double u;
    double v;

    YuvColor() : y(0), u(0), v(0) { }
    YuvColor(double y, double u, double v) : y(y), u(u), v(v) { }
    YuvColor(u8 y, u8 u, u8 v) : y(y / 255.0), u(u / 255.0), v(v / 255.0) { }
    YuvColor(u32 abgr) : YuvColor(u8(abgr), u8(abgr >> 8), u8(abgr >> 16)) {}
    YuvColor(const RgbColor &c);
    YuvColor(const GpuColor &c) : YuvColor(RgbColor(c)) { }

    static const YuvColor black;
    static const YuvColor white;
    static const YuvColor red;
    static const YuvColor green;
    static const YuvColor blue;
    static const YuvColor yellow;
    static const YuvColor magenta;
    static const YuvColor cyan;

    bool operator==(const YuvColor &rhs) const {
        return y == rhs.y && u == rhs.u && v == rhs.v;
    }

    YuvColor mix(const YuvColor &other, double weight) const;
    YuvColor tint(double weight) const { return mix(white, weight); }
    YuvColor shade(double weight) const { return mix(black, weight); }
};

struct GpuColor {

    u32 rawValue;

    GpuColor() : rawValue(0) {}
    GpuColor(u32 v) : rawValue(v) {}
    GpuColor(u64 v) : rawValue(u32(v)) {}
    GpuColor(u8 r, u8 g, u8 b);
    GpuColor(const RgbColor &c);
    GpuColor(const YuvColor &c) : GpuColor(RgbColor(c)) { }

    u8 r() const { return u8(rawValue       & 0xFF); }
    u8 g() const { return u8(rawValue >> 8  & 0xFF); }
    u8 b() const { return u8(rawValue >> 16 & 0xFF); }
    u8 a() const { return u8(rawValue >> 24 & 0xFF); }

    static const GpuColor black;
    static const GpuColor white;
    static const GpuColor red;
    static const GpuColor green;
    static const GpuColor blue;
    static const GpuColor yellow;
    static const GpuColor magenta;
    static const GpuColor cyan;

    operator u32() const { return rawValue; }

    bool operator==(const GpuColor &rhs) const {
        return rawValue == rhs.rawValue;
    }

    GpuColor mix(const RgbColor &other, double weight) const;
    GpuColor tint(double weight) const { return mix(RgbColor::white, weight); }
    GpuColor shade(double weight) const { return mix(RgbColor::black, weight); }
};

}

