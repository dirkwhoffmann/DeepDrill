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
#include "Buffer.h"
#include "StandardComplex.h"
#include "ColorMap.h"

namespace dd {

enum ChannelID {

    CHANNEL_ITCOUNTS,
    CHANNEL_LOGNORMS,
    CHANNEL_DERIVATIVES,
    CHANNEL_NORMALS
};

enum ChannelFormat {

    FMT_U24_LE,
    FMT_U32_LE,
    FMT_U64_LE,
    FMT_FP16_LE,
    FMT_FLOAT_LE,
    FMT_DOUBLE_LE
};

struct MapEntry {

    u32 iteration;
    float lognorm;

    // Experimental
    StandardComplex derivative;
    StandardComplex normal;
};

class DrillMap {

public:
    
    // Configuration options
    const struct Options &opt;

    // Resolution
    isize width = 0;
    isize height = 0;

    // Map data (TODO: Use Buffer class)
    MapEntry *data = nullptr;

    // Associated color map
    ColorMap colorMap = ColorMap(opt);

public:


    //
    // Initializing
    //

    DrillMap(const Options &opt) : opt(opt) { };
    ~DrillMap();

    void resize();
    void resize(isize w, isize h);


    //
    // Analyzing
    //

    bool hasIterations();
    bool hasLogNorms();
    bool hasDerivates();
    bool hasNormals();


    //
    // Accessing
    //

    MapEntry *operator [] (const isize &) const;
    MapEntry &get(isize w, isize h) const;
    MapEntry &get(const struct Coord &c) const;
    void set(isize w, isize h, const MapEntry &entry);
    void set(const struct Coord &c, const MapEntry &entry);
    void set(const struct Coord &c, u32 iteration, float lognorm);


    //
    // Colorizing
    //

    ColorMap &getColorMap() { return colorMap; }
    const sf::Texture &getTexture() { return colorMap.getTexture(); }
    const ColorMap &colorize();


    //
    // Loading
    //

public:

    void load(const string &path);
    void load(std::istream &is);

private:

    void loadHeader(std::istream &is);
    void loadChannel(std::istream &is);
    template<ChannelFormat fmt, typename T> void load(std::istream &is, T &raw);
    template<ChannelFormat fmt> void load(std::istream &is, u32 &raw) { load <fmt, u32> (is, raw); }
    template<ChannelFormat fmt> void load(std::istream &is, u64 &raw) { load <fmt, u64> (is, raw); }
    template<ChannelFormat fmt> void load(std::istream &is, float &raw) { load <fmt, float> (is, raw); }
    template<ChannelFormat fmt> void load(std::istream &is, double &raw) { load <fmt, double> (is, raw); }
    template<ChannelFormat fmt> void load(std::istream &is, StandardComplex &raw);

    
    //
    // Saving
    //

public:

    void save(const string &path);
    void save(std::ostream &os);

private:

    void saveHeader(std::ostream &os);
    void saveChannel(std::ostream &os, ChannelID id);
    template<ChannelFormat fmt, typename T> void save(std::ostream &os, T raw);
    template<ChannelFormat fmt> void save(std::ostream &os, u32 raw) { save <fmt, u32> (os, raw); }
    template<ChannelFormat fmt> void save(std::ostream &os, u64 raw) { save <fmt, u64> (os, raw); }
    template<ChannelFormat fmt> void save(std::ostream &os, float raw) { save <fmt, float> (os, raw); }
    template<ChannelFormat fmt> void save(std::ostream &os, double raw) { save <fmt, double> (os, raw); }
    template<ChannelFormat fmt> void save(std::ostream &os, const StandardComplex &raw);
};

}
