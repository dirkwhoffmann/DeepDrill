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

DrillMap::DrillMap(const Options &o) : opt(o)
{
    resize(opt.image.width, opt.image.height);
}

void
DrillMap::resize(isize w, isize h)
{
    assert(w <= 3840);
    assert(h <= 2160);

    if (data) delete [] data;
    
    width = w;
    height = h;
    data = new MapEntry[w * h];
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
DrillMap::set(const struct Coord &c, u32 iteration, float lognorm)
{
    set(c.x, c.y, MapEntry { iteration, (float)::log(lognorm), StandardComplex(), StandardComplex() } );
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
    ProgressIndicator progress("Loading map file");

    // Load header
    loadHeader(is);

    // Adjust the map size
    resize(width, height);

    // Read channels
    while (is.peek() != EOF) { loadChannel(is); }

    progress.done();

    if (opt.flags.verbose) {

        log::cout << log::vspace;
        log::cout << log::ralign("Image size: ");
        log::cout << "(" << width << "," << height << ")" << log::endl;
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

        case CHANNEL_ITCOUNT:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    switch (ChannelFormat(fmt)) {

                        case FMT_U32_LE: load<FMT_U32_LE> (is, get(x,y).iteration); break;
                        case FMT_U64_LE: load<FMT_U64_LE> (is, get(x,y).iteration); break;

                        default:
                            throw Exception("Invalid data format");
                    }
                }
            }
            break;

        case CHANNEL_LOGNORM:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    switch (ChannelFormat(fmt)) {

                        case FMT_FLOAT_LE: load<FMT_FLOAT_LE> (is, get(x,y).lognorm); break;
                        case FMT_DOUBLE_LE: load<FMT_DOUBLE_LE> (is, get(x,y).lognorm); break;

                        default:
                            throw Exception("Invalid data format");
                    }
                }

                // if (y == 0) printf("Loading lognorm %f\n", get(x,y).lognorm);

            }
            break;

        case CHANNEL_NORMALS:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    switch (ChannelFormat(fmt)) {

                        case FMT_FLOAT_LE:

                            load <FMT_FLOAT_LE> (is, get(x,y).normal.re);
                            load <FMT_FLOAT_LE> (is, get(x,y).normal.im);
                            break;

                        case FMT_DOUBLE_LE:

                            load <FMT_DOUBLE_LE> (is, get(x,y).normal.re);
                            load <FMT_DOUBLE_LE> (is, get(x,y).normal.im);
                            break;

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
    ProgressIndicator progress("Saving map file");

    // Write header
    saveHeader(os);

    // Write channels
    saveChannel(os, CHANNEL_ITCOUNT);
    saveChannel(os, CHANNEL_LOGNORM);
    saveChannel(os, CHANNEL_NORMALS);
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

        case CHANNEL_ITCOUNT:

            os << u8(id) << u8(FMT_U32_LE);

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    save <FMT_U32_LE> (os, get(x,y).iteration);
                }
            }
            break;

        case CHANNEL_LOGNORM:

            os << u8(id) << u8(FMT_FLOAT_LE);

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    save <FMT_FLOAT_LE> (os, get(x,y).lognorm);

                    if (y == 0) printf("Saving lognorm %f\n", get(x,y).lognorm);
                }
            }
            break;

        case CHANNEL_NORMALS:

            os << u8(id) << u8(FMT_FLOAT_LE);

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    save <FMT_FLOAT_LE> (os, get(x,y).normal.re);
                    save <FMT_FLOAT_LE> (os, get(x,y).normal.im);
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

}
