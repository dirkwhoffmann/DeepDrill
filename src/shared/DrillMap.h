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
#include "StandardComplex.h"
#include "ColorMap.h"
#include "Compressor.h"

namespace dd {

enum DrillResult : i8 {

    DR_UNPROCESSED,
    DR_ESCAPED,
    DR_MAX_DEPTH_REACHED,
    DR_IN_BULB,
    DR_IN_CARDIOID,
    DR_PERIODIC,
    DR_ATTRACTED,
    DR_GLITCH,
};

enum ChannelFormat {

    FMT_I8,
    FMT_I16,
    FMT_I24,
    FMT_I32,
    FMT_FP16,
    FMT_FLOAT,
    FMT_DOUBLE
};

enum ChannelID {

    CHANNEL_RESULT,
    CHANNEL_FIRST,
    CHANNEL_LAST,
    CHANNEL_LOGNORM,
    CHANNEL_DERIVATIVE,
    CHANNEL_NORMAL
};

struct MapEntry {

    DrillResult result;

    // First executed iteration
    i32 first;

    // Last executed iteration
    i32 last;

    // What is the best format to save here
    float lognorm;

    // Derivative
    StandardComplex derivative;

    // Normal vector
    StandardComplex normal;
};

class DrillMap {

public:
    
    // Configuration options
    const struct Options &opt;

    // Resolution
    isize width = 0;
    isize height = 0;
    isize depth = 0;

    // Distance between adjacent pixels
    mpf_class mpfPixelDeltaX;
    mpf_class mpfPixelDeltaY;
    ExtendedDouble pixelDeltaX;
    ExtendedDouble pixelDeltaY;

    // Map data (TODO: Use std::vector<MapEntry>)
    // MapEntry *data = nullptr;
    std::vector<MapEntry> data;

    // Associated color map
    ColorMap colorMap = ColorMap(opt);


    //
    // Initializing
    //

public:

    DrillMap(const Options &opt) : opt(opt) { };
    // ~DrillMap() { };

    void resize();
    void resize(isize w, isize h, isize d);


    //
    // Accessing
    //

public:

    MapEntry *operator [] (const isize &);
    const MapEntry *operator [] (const isize &) const;
    MapEntry &get(isize w, isize h);
    const MapEntry &get(isize w, isize h) const;
    MapEntry &get(const struct Coord &c);
    const MapEntry &get(const struct Coord &c) const;
    void set(isize w, isize h, const MapEntry &entry);
    void set(const struct Coord &c, const MapEntry &entry);


    //
    // Measuring
    //

    // Translates a coordinate into a complex number and vice versa
    PrecisionComplex translate(const Coord &coord) const;
    Coord translate(const PrecisionComplex &coord) const;

    // Returns the coordinates of a mesh covering the drill map
    void getMesh(isize numx, isize numy, std::vector<Coord> &meshPoints) const;


    //
    // Analyzing
    //

public:

    bool hasDrillResults() const;
    bool hasIterations() const;
    bool hasLogNorms() const;
    bool hasDerivates() const;
    bool hasNormals() const;
    void analyze() const;


    //
    // Colorizing
    //

public:

    ColorMap &getColorMap() { return colorMap; }
    const ColorMap &colorize();


    //
    // Loading
    //

public:

    void load(const string &path);
    void load(std::istream &is);

private:

    void loadHeader(std::istream &is);
    void loadChannel(Compressor &is);
    template<ChannelFormat fmt, typename T> void load(Compressor &is, T &raw);
    template<ChannelFormat fmt> void load(Compressor &is, u32 &raw) { load <fmt, u32> (is, raw); }
    template<ChannelFormat fmt> void load(Compressor &is, u64 &raw) { load <fmt, u64> (is, raw); }
    template<ChannelFormat fmt> void load(Compressor &is, float &raw) { load <fmt, float> (is, raw); }
    template<ChannelFormat fmt> void load(Compressor &is, double &raw) { load <fmt, double> (is, raw); }

    
    //
    // Saving
    //

public:

    void save(const string &path);
    void save(std::ostream &os);

private:

    void saveHeader(std::ostream &os);
    void saveChannel(Compressor &os, ChannelID id);
    template<ChannelFormat fmt, typename T> void save(Compressor &os, T raw);
    template<ChannelFormat fmt> void save(Compressor &os, u32 raw) { save <fmt, u32> (os, raw); }
    template<ChannelFormat fmt> void save(Compressor &os, u64 raw) { save <fmt, u64> (os, raw); }
    template<ChannelFormat fmt> void save(Compressor &os, float raw) { save <fmt, float> (os, raw); }
    template<ChannelFormat fmt> void save(Compressor &os, double raw) { save <fmt, double> (os, raw); }
};

}
