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
#include "ExtendedComplex.h"
#include "PrecisionComplex.h"
#include "Compressor.h"

#include <SFML/Graphics.hpp>

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

    CHANNEL_RESULT,     // Pixel type
    CHANNEL_FIRST,      // First executed iteration
    CHANNEL_LAST,       // Last executed iteration
    CHANNEL_NITCNT,     // Normalized iteration count
    CHANNEL_DERIVATIVE, // Derivative
    CHANNEL_NORMAL,     // Normal vector
    CHANNEL_DIST,       // Distance estimates
};

struct MapEntry {

    // Drill outcome
    DrillResult result;

    // First executed iteration
    i32 first;

    // Last executed iteration
    i32 last;

    // Last iteration value before the escape check hit
    StandardComplex zn;
    
    // Derivative
    StandardComplex derivative;

    // Normal vector
    StandardComplex normal;
};

class DrillMap {

public:
    
    // Map resolution
    isize width = 0;
    isize height = 0;
    isize depth = 0;

    // The center coordinate
    PrecisionComplex center;

    // Bounding box
    PrecisionComplex ul;
    PrecisionComplex lr;

    // Distance between adjacent pixels
    mpf_class mpfPixelDeltaX;
    mpf_class mpfPixelDeltaY;
    ExtendedDouble pixelDeltaX;
    ExtendedDouble pixelDeltaY;

    // Map data
    std::vector<DrillResult> resultMap;
    std::vector<u32> firstIterationMap;
    std::vector<u32> lastIterationMap;
    std::vector<u32> overlayMap;
    std::vector<u32> textureMap;
    std::vector<float> nitcntMap;
    std::vector<float> distMap;
    std::vector<double> derivReMap;
    std::vector<double> derivImMap;
    std::vector<float> normalReMap;
    std::vector<float> normalImMap;

    // Map data in texture format
    sf::Texture iterationMapTex;
    sf::Texture overlayMapTex;
    sf::Texture nitcntMapTex;
    sf::Texture distMapTex;
    sf::Texture normalReMapTex;
    sf::Texture normalImMapTex;

    // Indicates whether texture maps are dirty
    bool dirty = true;

    const sf::Texture &getIterationMapTex() { updateTextures(); return iterationMapTex; }
    const sf::Texture &getOverlayMapTex() { updateTextures(); return overlayMapTex; }
    const sf::Texture &getNitcntMapTex() { updateTextures(); return nitcntMapTex; }
    const sf::Texture &getDistMapTex() { updateTextures(); return distMapTex; }
    const sf::Texture &getNormalReMapTex() { updateTextures(); return normalReMapTex; }
    const sf::Texture &getNormalImMapTex() { updateTextures(); return normalImMapTex; }


    //
    // Initializing
    //

public:

    void resize();
    void resize(isize w, isize h, isize d);


    //
    // Accessing
    //

public:

    void set(isize w, isize h, const MapEntry &entry);
    void set(const struct Coord &c, const MapEntry &entry);


    //
    // Locating
    //

    // Translates a coordinate into a complex number and vice versa
    PrecisionComplex translate(const Coord &coord) const;
    Coord translate(const PrecisionComplex &coord) const;

    // Computes the distance of coordinate from another coordinate
    ExtendedComplex distance(const Coord &coord, const Coord &other) const;

    // Computes the distance from the center
    ExtendedComplex distance(const Coord &coord) const;

    // Returns the coordinates of a mesh covering the drill map
    void getMesh(isize numx, isize numy, std::vector<Coord> &meshPoints) const;


    //
    // Analyzing
    //

public:

    bool hasDrillResults() const;
    bool hasIterations() const;
    bool hasNormalizedIterationCounts() const;
    bool hasDistances() const;
    bool hasDerivates() const;
    bool hasNormals() const;
    void analyze() const;


    //
    // Colorizing
    //

public:

    void updateTextures();


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
    template<ChannelFormat fmt> void load(Compressor &is, u32 &raw) { load <fmt,u32> (is, raw); }
    template<ChannelFormat fmt> void load(Compressor &is, u64 &raw) { load <fmt,u64> (is, raw); }
    template<ChannelFormat fmt> void load(Compressor &is, float &raw) { load <fmt,float> (is, raw); }
    template<ChannelFormat fmt> void load(Compressor &is, double &raw) { load <fmt,double> (is, raw); }

    
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
    template<ChannelFormat fmt> void save(Compressor &os, u32 raw) { save <fmt,u32> (os, raw); }
    template<ChannelFormat fmt> void save(Compressor &os, u64 raw) { save <fmt,u64> (os, raw); }
    template<ChannelFormat fmt> void save(Compressor &os, float raw) { save <fmt,float> (os, raw); }
    template<ChannelFormat fmt> void save(Compressor &os, double raw) { save <fmt,double> (os, raw); }
};

}
