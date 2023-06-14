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

#include "DrillMap.h"
#include "Coord.h"
#include "Exception.h"
#include "IO.h"
#include "Logger.h"
#include "Options.h"
#include "ProgressIndicator.h"
#include "StandardComplex.h"

namespace dd {

DrillMap::~DrillMap()
{
    delete [] data;
}

void
DrillMap::resize()
{
    resize(opt.drillmap.width, opt.drillmap.height);
}

void
DrillMap::resize(isize w, isize h)
{
    assert(w >= MIN_MAP_WIDTH && w <= MAX_MAP_WIDTH);
    assert(h >= MIN_MAP_HEIGHT && h <= MAX_MAP_HEIGHT);

    if (data) delete [] data;
    
    width = w;
    height = h;
    data = new MapEntry[w * h] ();

    assert(!hasIterations());
    assert(!hasLogNorms());
    assert(!hasDerivates());
    assert(!hasNormals());
}

bool
DrillMap::hasIterations()
{
    for (isize i = 0; i < width * height; i++) {
        if (data[i].iteration) return true;
    }
    return false;
}

bool
DrillMap::hasLogNorms()
{
    for (isize i = 0; i < width * height; i++) {
        if (data[i].lognorm) return true;
    }
    return false;
}

bool
DrillMap::hasDerivates()
{
    for (isize i = 0; i < width * height; i++) {
        if (data[i].derivative != StandardComplex(0,0)) return true;
    }
    return false;
}

bool
DrillMap::hasNormals()
{
    for (isize i = 0; i < width * height; i++) {
        if (data[i].normal != StandardComplex(0,0)) return true;
    }
    return false;
}

MapEntry *
DrillMap::operator [] (const isize &index) const
{
    assert(index < height);
    return data + (index * width);
}

MapEntry &
DrillMap::get(isize w, isize h) const
{
    assert(data != nullptr && w < width && h < height);
    return data[h * width + w];
}

MapEntry &
DrillMap::get(const struct Coord &c) const
{
    return get(c.x, c.y);
}

void
DrillMap::set(isize w, isize h, const MapEntry &entry)
{
    assert(data != nullptr && w < width && h < height);
    data[h * width + w] = entry;
}

void
DrillMap::set(const struct Coord &c, const MapEntry &entry)
{
    set(c.x, c.y, entry);
}

void
DrillMap::set(const struct Coord &c, i32 iteration, float lognorm)
{
    set(c.x, c.y, MapEntry { iteration, (float)::log(lognorm), StandardComplex(), StandardComplex() } );
}

void
DrillMap::markAsInside(const struct Coord &c)
{
    set(c.x, c.y, MapEntry {
        0,
        INFINITY,
        StandardComplex(),
        StandardComplex()
    } );
}

void
DrillMap::markAsGlitch(const struct Coord &c)
{
    set(c.x, c.y, MapEntry {
        INT32_MAX,
        NAN,
        StandardComplex(),
        StandardComplex()
    } );
}

void
DrillMap::markAsRejected(const struct Coord &c)
{
    set(c.x, c.y, MapEntry {
        0,
        INFINITY,
        StandardComplex(),
        StandardComplex()
    } );
}

void
DrillMap::getMesh(isize numx, isize numy, std::vector <Coord> &mesh)
{
    // The coordinate system is superimposed with an equidistant mesh.
    // The density of the mesh is controlled by the 'numx' and 'numy'.
    // The minimum value is 2 which produces a mesh that comprises the four
    // corner points.

    mesh.clear();

    for (isize i = 0; i < numx; i++) {
        for (isize j = 0; j < numy; j++) {

            auto x = (width - 1) * ((double)i / (double)(numx - 1));
            auto y = (height - 1) * ((double)j / (double)(numy - 1));

            mesh.push_back(Coord(x,y));
        }
    }
}

const ColorMap &
DrillMap::colorize()
{
    colorMap.compute(*this);
    return colorMap;
}

void
DrillMap::load(const string &path)
{
    std::ifstream os(path.c_str(), std::ios::binary);
    if (!os.is_open()) throw Exception("Failed to read file " + path);

    load(os);
}

void
DrillMap::load(std::istream &is)
{
    {
        ProgressIndicator progress("Loading map file");

        // Load header
        loadHeader(is);

        // Adjust the map size
        resize(width, height);

        // Read channels
        while (is.peek() != EOF) { loadChannel(is); }
    }

    if (opt.flags.verbose) {

        log::cout << log::vspace;
        log::cout << log::ralign("Map size: ");
        log::cout << width << " x " << height << log::endl;
        log::cout << log::ralign("Iteration counts: ");
        log::cout << (hasIterations() ? "Loaded" : "Not included in map file") << log::endl;
        log::cout << log::ralign("Lognorms: ");
        log::cout << (hasLogNorms() ? "Loaded" : "Not included in map file") << log::endl;
        log::cout << log::ralign("Derivates: ");
        log::cout << (hasDerivates() ? "Loaded" : "Not included in map file") << log::endl;
        log::cout << log::ralign("Normals: ");
        log::cout << (hasNormals() ? "Loaded" : "Not included in map file") << log::endl;
        log::cout << log::vspace;
    }
}

void
DrillMap::loadHeader(std::istream &is)
{
    char magicBytes[10] = { };

    // Magic bytes
    is.read(magicBytes, 9);
    if (string(magicBytes) != "DeepDrill") {
        throw Exception("Not a valid map file. Invalid header.");
    }

    // Version number
    u8 major; is >> major;
    u8 minor; is >> minor;
    u8 beta;  is >> beta;

    // Width and height
    is.read((char *)&width, sizeof(width));
    is.read((char *)&height, sizeof(height));
}

void
DrillMap::loadChannel(std::istream &is)
{
    u8 id, fmt;

    is >> id;
    is >> fmt;

    switch (ChannelID(id)) {

        case CHANNEL_ITCOUNTS:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    switch (ChannelFormat(fmt)) {

                        case FMT_U24_LE: load<FMT_U24_LE> (is, get(x,y).iteration); break;
                        case FMT_U32_LE: load<FMT_U32_LE> (is, get(x,y).iteration); break;
                        case FMT_U64_LE: load<FMT_U64_LE> (is, get(x,y).iteration); break;

                        default:
                            throw Exception("Invalid data format");
                    }
                }
            }
            break;

        case CHANNEL_LOGNORMS:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    switch (ChannelFormat(fmt)) {

                        case FMT_FP16_LE: load<FMT_FP16_LE> (is, get(x,y).lognorm); break;
                        case FMT_FLOAT_LE: load<FMT_FLOAT_LE> (is, get(x,y).lognorm); break;
                        case FMT_DOUBLE_LE: load<FMT_DOUBLE_LE> (is, get(x,y).lognorm); break;

                        default:
                            throw Exception("Invalid data format");
                    }
                }
            }
            break;

        case CHANNEL_DERIVATIVES:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    switch (ChannelFormat(fmt)) {

                        case FMT_FP16_LE: load<FMT_FP16_LE> (is, get(x,y).derivative); break;
                        case FMT_FLOAT_LE: load<FMT_FLOAT_LE> (is, get(x,y).derivative); break;
                        case FMT_DOUBLE_LE: load<FMT_DOUBLE_LE> (is, get(x,y).derivative); break;

                        default:
                            throw Exception("Invalid data format");
                    }
                }
            }
            break;

        case CHANNEL_NORMALS:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    switch (ChannelFormat(fmt)) {

                        case FMT_FP16_LE: load<FMT_FP16_LE> (is, get(x,y).normal); break;
                        case FMT_FLOAT_LE: load<FMT_FLOAT_LE> (is, get(x,y).normal); break;
                        case FMT_DOUBLE_LE: load<FMT_DOUBLE_LE> (is, get(x,y).normal); break;

                        default:
                            throw Exception("Invalid data format");
                    }
                }
            }
            break;

        default:

            throw Exception("Invalid channel ID: " + std::to_string(id));
    }
}

template<ChannelFormat fmt, typename T> void
DrillMap::load(std::istream &is, T &raw)
{
    switch (fmt) {

        case FMT_U24_LE:
        {
            u32 value = 0;
            is.read((char *)&value, 3);
            raw = (T)value;
            break;
        }
        case FMT_U32_LE:
        {
            u32 value;
            is.read((char *)&value, sizeof(value));
            raw = (T)value;
            break;
        }
        case FMT_U64_LE:
        {
            u64 value;
            is.read((char *)&value, sizeof(value));
            raw = (T)value;
            break;
        }
        case FMT_FP16_LE:
        {
            i16 value;
            is.read((char *)&value, sizeof(value));
            raw = (T)value / (T)INT16_MAX;
            break;
        }
        case FMT_FLOAT_LE:
        {
            float value;
            is.read((char *)&value, sizeof(value));
            raw = (T)value;
            break;
        }
        case FMT_DOUBLE_LE:
        {
            double value;
            is.read((char *)&value, sizeof(value));
            raw = (T)value;
            break;
        }
    }
}

template<ChannelFormat fmt> void
DrillMap::load(std::istream &is, StandardComplex &raw)
{
    load <fmt> (is, raw.re);
    load <fmt> (is, raw.im);
}

void
DrillMap::save(const string &path)
{
    std::ofstream os(path.c_str(), std::ios::binary);
    if (!os.is_open()) throw Exception("Failed to write file " + path);

    save(os);
}

void
DrillMap::save(std::ostream &os)
{
    bool saveIterations = true;
    bool saveLognorms = true;
    bool saveDerivatives = false;
    bool saveNormals = opt.drillmap.depth == 1;

    {
        ProgressIndicator progress("Saving map file");

        // Write header
        saveHeader(os);

        // Write channels
        if (saveIterations) saveChannel(os, CHANNEL_ITCOUNTS);
        if (saveLognorms) saveChannel(os, CHANNEL_LOGNORMS);
        if (saveDerivatives) saveChannel(os, CHANNEL_DERIVATIVES);
        if (saveNormals) saveChannel(os, CHANNEL_NORMALS);
    }
    
    if (opt.flags.verbose) {

        log::cout << log::vspace;
        log::cout << log::ralign("Map size: ");
        log::cout << width << " x " << height << log::endl;
        log::cout << log::ralign("Iteration counts: ");
        log::cout << (saveIterations ? "Saved" : "Not saved") << log::endl;
        log::cout << log::ralign("Lognorms: ");
        log::cout << (saveLognorms ? "Saved" : "Not saved") << log::endl;
        log::cout << log::ralign("Derivatives: ");
        log::cout << (saveDerivatives ? "Saved" : "Not saved") << log::endl;
        log::cout << log::ralign("Normals: ");
        log::cout << (saveNormals ? "Saved" : "Not saved") << log::endl;
        log::cout << log::vspace;
    }
}

void
DrillMap::saveHeader(std::ostream &os)
{
    // Magic bytes
    os.write("DeepDrill", 9);

    // Version number
    os << u8(VER_MAJOR);
    os << u8(VER_MINOR);
    os << u8(VER_BETA);

    // Width and height
    os.write((char *)&width, sizeof(width));
    os.write((char *)&height, sizeof(height));
}

void
DrillMap::saveChannel(std::ostream &os, ChannelID id)
{
    switch (id) {

        case CHANNEL_ITCOUNTS:

            os << u8(id) << u8(FMT_U32_LE);

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    save <FMT_U32_LE> (os, get(x,y).iteration);
                }
            }
            break;

        case CHANNEL_LOGNORMS:

            os << u8(id) << u8(FMT_FLOAT_LE);

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    save <FMT_FLOAT_LE> (os, get(x,y).lognorm);
                }
            }
            break;

        case CHANNEL_DERIVATIVES:

            os << u8(id) << u8(FMT_FLOAT_LE);

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    save <FMT_FLOAT_LE> (os, get(x,y).derivative);
                }
            }
            break;

        case CHANNEL_NORMALS:

            os << u8(id) << u8(FMT_FP16_LE);

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    save <FMT_FP16_LE> (os, get(x,y).normal);
                }
            }
            break;

        default:

            throw Exception("Invalid channel ID: " + std::to_string(id));
    }
}

template<ChannelFormat fmt, typename T> void
DrillMap::save(std::ostream &os, T raw)
{
    switch (fmt) {

        case FMT_U24_LE:
        {
            u32 value = (u32)raw;
            os.write((char *)&value, 3);
            break;
        }
        case FMT_U32_LE:
        {
            u32 value = (u32)raw;
            os.write((char *)&value, sizeof(value));
            break;
        }
        case FMT_U64_LE:
        {
            u64 value = (u64)raw;
            os.write((char *)&value, sizeof(value));
            break;
        }
        case FMT_FP16_LE:
        {
            i16 value = (i16)(raw * (T)INT16_MAX);
            os.write((char *)&value, sizeof(value));
            break;
        }
        case FMT_FLOAT_LE:
        {
            float value = (float)raw;
            os.write((char *)&value, sizeof(value));
            break;
        }
        case FMT_DOUBLE_LE:
        {
            double value = (double)raw;
            os.write((char *)&value, sizeof(value));
            break;
        }
    }
}

template<ChannelFormat fmt> void
DrillMap::save(std::ostream &os, const StandardComplex &raw)
{
    save <fmt> (os, raw.re);
    save <fmt> (os, raw.im);
}

}
