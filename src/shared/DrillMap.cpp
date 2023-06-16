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
#include "Application.h"
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
DrillMap::hasDrillResults() const
{
    for (isize i = 0; i < width * height; i++) {
        if (data[i].result) return true;
    }
    return false;
}

bool
DrillMap::hasIterations() const
{
    for (isize i = 0; i < width * height; i++) {
        if (data[i].iteration) return true;
    }
    return false;
}

bool
DrillMap::hasLogNorms() const
{
    for (isize i = 0; i < width * height; i++) {
        if (data[i].lognorm) return true;
    }
    return false;
}

bool
DrillMap::hasDerivates() const
{
    for (isize i = 0; i < width * height; i++) {
        if (data[i].derivative != StandardComplex(0,0)) return true;
    }
    return false;
}

bool
DrillMap::hasNormals() const
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
DrillMap::set(const struct Coord &c, i32 it, float lognorm)
{
    set(c.x, c.y, MapEntry { DR_ESCAPED, it, (float)::log(lognorm), StandardComplex(), StandardComplex() } );
}

void
DrillMap::set(const struct Coord &c, DrillResult dr, i32 it)
{
    set(c.x, c.y, MapEntry { dr, it, 0.0, StandardComplex(), StandardComplex() } );
}

/*
void
DrillMap::markAsInside(const struct Coord &c, PointType type)
{
    set(c.x, c.y, MapEntry { type, 0.0, StandardComplex(), StandardComplex() } );
}
*/

void
DrillMap::getMesh(isize numx, isize numy, std::vector<Coord> &mesh)
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
    ProgressIndicator progress1("Loading map file");

    // Load header
    loadHeader(is);

    // Adjust the map size
    resize(width, height);

    // The next byte indicates if the map is compressed
    u8 compressed; is >> compressed;

    // Load the rest of the file
    Compressor compressor(width * height * sizeof(MapEntry));
    compressor << is;
    progress1.done();

    // Uncompress the channel data if needed
    if (compressed) {

        ProgressIndicator progress2("Uncompressing map file");
        compressor.uncompressData();
    }


    // Extract all channels
    ProgressIndicator progress3("Extracting channels");
    while (!compressor.eof()) { loadChannel(compressor); }
    progress3.done();

    if (opt.flags.verbose) {

        log::cout << log::vspace;
        log::cout << log::ralign("Map size: ");
        log::cout << width << " x " << height << log::endl;
        log::cout << log::ralign("Drill results: ");
        log::cout << (hasDrillResults() ? "Loaded" : "Not included in map file") << log::endl;
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

    // Version number and map format
    u8 major;       is >> major;
    u8 minor;       is >> minor;
    u8 subminor;    is >> subminor;
    u8 beta;        is >> beta;
    u8 format;      is >> format;

    // Only proceed if the mapfile format is understood
    if (format != MAP_FORMAT) {
        throw Exception("The mapfile is incompatible with this release. It has been generated with DeepDrill " +
                        Application::version(major, minor, subminor, beta) +
                        ".");
    }

    // Width and height
    is.read((char *)&width, sizeof(width));
    is.read((char *)&height, sizeof(height));
}

void
DrillMap::loadChannel(Compressor &is)
{
    u8 id, fmt;

    is >> id;
    is >> fmt;

    switch (ChannelID(id)) {

        case CHANNEL_DRILLRESULT:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    i8 value;
                    load <FMT_I8> (is, value);
                    get(x,y).result = DrillResult(value);

                }
            }
            break;

        case CHANNEL_ITCOUNTS:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    switch (ChannelFormat(fmt)) {

                        case FMT_I8:  load <FMT_I8>  (is, get(x,y).iteration); break;
                        case FMT_I16: load <FMT_I16> (is, get(x,y).iteration); break;
                        case FMT_I24: load <FMT_I24> (is, get(x,y).iteration); break;
                        case FMT_I32: load <FMT_I32> (is, get(x,y).iteration); break;

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

                        case FMT_FP16: load <FMT_FP16> (is, get(x,y).lognorm); break;
                        case FMT_FLOAT: load <FMT_FLOAT> (is, get(x,y).lognorm); break;
                        case FMT_DOUBLE: load <FMT_DOUBLE> (is, get(x,y).lognorm); break;

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

                        case FMT_FP16: load <FMT_FP16> (is, get(x,y).derivative); break;
                        case FMT_FLOAT: load <FMT_FLOAT> (is, get(x,y).derivative); break;
                        case FMT_DOUBLE: load <FMT_DOUBLE> (is, get(x,y).derivative); break;

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

                        case FMT_FP16: load <FMT_FP16> (is, get(x,y).normal); break;
                        case FMT_FLOAT: load <FMT_FLOAT> (is, get(x,y).normal); break;
                        case FMT_DOUBLE: load <FMT_DOUBLE> (is, get(x,y).normal); break;

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
DrillMap::load(Compressor &is, T &raw)
{
    switch (fmt) {

        case FMT_I8:
        {
            i8 value;
            is >> value;
            raw = (T)value;
            break;
        }
        case FMT_I16:
        {
            i16 value;
            is >> value;
            raw = (T)value;
            break;
        }
        case FMT_I24:
        {
            u8 b[3];
            is >> b[2]; is >> b[1]; is >> b[0];
            raw = T(((i32)(i8)b[2] << 16) + (b[1] << 8) + b[0]);
            break;
        }
        case FMT_I32:
        {
            i32 value;
            is >> value;
            raw = (T)value;
            break;

        }
        case FMT_FP16:
        {
            i16 value;
            is >> value;
            raw = (T)value / (T)INT16_MAX;
            break;
        }
        case FMT_FLOAT:
        {
            float value;
            is >> value;
            raw = (T)value;
            break;
        }
        case FMT_DOUBLE:
        {
            double value;
            is >> value;
            raw = (T)value;
            break;
        }
    }
}

template<ChannelFormat fmt> void
DrillMap::load(Compressor &is, StandardComplex &raw)
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
    bool saveDrillResults = true;
    bool saveIterations = true;
    bool saveLognorms = true;
    bool saveDerivatives = false;
    bool saveNormals = opt.drillmap.depth == 1;

    ProgressIndicator progress1("Preparing map file");

    // Write header
    saveHeader(os);

    // The next byte indicates if channel data is compressed
    os << u8(opt.drillmap.compress);

    // Generate channels
    Compressor compressor(width * height * sizeof(MapEntry));
    if (saveDrillResults) saveChannel(compressor, CHANNEL_DRILLRESULT);
    if (saveIterations) saveChannel(compressor, CHANNEL_ITCOUNTS);
    if (saveLognorms) saveChannel(compressor, CHANNEL_LOGNORMS);
    if (saveDerivatives) saveChannel(compressor, CHANNEL_DERIVATIVES);
    if (saveNormals) saveChannel(compressor, CHANNEL_NORMALS);
    progress1.done();

    if (opt.flags.verbose) {

        log::cout << log::vspace;
        log::cout << log::ralign("Map size: ");
        log::cout << width << " x " << height << log::endl;
        log::cout << log::ralign("Drill results: ");
        log::cout << (saveDrillResults ? "Saved" : "Not saved") << log::endl;
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

    if (opt.drillmap.compress) {

        ProgressIndicator progress2("Compressing map file");

        auto oldSize = compressor.size();
        compressor.compressData();
        auto newSize = compressor.size();
        progress2.done();

        if (opt.flags.verbose) {

            log::cout << log::vspace;
            log::cout << log::ralign("Reduction: ");
            log::cout << isize(100.0 * newSize / oldSize) << "%" << log::endl;
            log::cout << log::vspace;
        }
    }

    ProgressIndicator progress3("Saving map file");
    compressor >> os;
    progress3.done();
}

void
DrillMap::saveHeader(std::ostream &os)
{
    // Magic bytes
    os.write("DeepDrill", 9);

    // Version number
    os << u8(VER_MAJOR);
    os << u8(VER_MINOR);
    os << u8(VER_SUBMINOR);
    os << u8(VER_BETA);
    os << u8(MAP_FORMAT);

    // Width and height
    os.write((char *)&width, sizeof(width));
    os.write((char *)&height, sizeof(height));
}

void
DrillMap::saveChannel(Compressor &os, ChannelID id)
{
    switch (id) {

        case CHANNEL_DRILLRESULT:

            os << u8(id) << u8(FMT_I8);

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    save <FMT_I8> (os, i8(get(x,y).result));
                }
            }
            break;

        case CHANNEL_ITCOUNTS:

            os << u8(id) << u8(FMT_I32);

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    save <FMT_I32> (os, get(x,y).iteration);
                }
            }
            break;

        case CHANNEL_LOGNORMS:

            os << u8(id) << u8(FMT_FLOAT);

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    save <FMT_FLOAT> (os, get(x,y).lognorm);
                }
            }
            break;

        case CHANNEL_DERIVATIVES:

            os << u8(id) << u8(FMT_FLOAT);

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    save <FMT_FLOAT> (os, get(x,y).derivative);
                }
            }
            break;

        case CHANNEL_NORMALS:

            os << u8(id) << u8(FMT_FP16);

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {

                    save <FMT_FP16> (os, get(x,y).normal);
                }
            }
            break;

        default:

            throw Exception("Invalid channel ID: " + std::to_string(id));
    }
}

template<ChannelFormat fmt, typename T> void
DrillMap::save(Compressor &os, T raw)
{
    switch (fmt) {

        case FMT_I8:

            os << (i8)raw;
            break;

        case FMT_I16:

            os << (i16)raw;
            break;

        case FMT_I24:

            os << (u8)((i32)raw >> 16);
            os << (u8)((i32)raw >> 8);
            os << (u8)((i32)raw);
            break;

        case FMT_I32:

            os << (i32)raw;
            break;

        case FMT_FP16:
        {
            i16 value = (i16)(raw * (T)INT16_MAX);
            os << value;
            break;
        }
        case FMT_FLOAT:
        {
            float value = (float)raw;
            os << value;
            break;
        }
        case FMT_DOUBLE:
        {
            double value = (double)raw;
            os << value;
            break;
        }
    }
}

template<ChannelFormat fmt> void
DrillMap::save(Compressor &os, const StandardComplex &raw)
{
    save <fmt> (os, raw.re);
    save <fmt> (os, raw.im);
}

}
