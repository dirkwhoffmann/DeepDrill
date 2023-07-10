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
#include "Options.h"
#include "Coord.h"
#include "IO.h"
#include "Logger.h"
#include "ProgressIndicator.h"
#include "StandardComplex.h"

namespace dd {

void
DrillMap::resize()
{
    resize(Options::drillmap.width, Options::drillmap.height, Options::drillmap.depth);
}

void
DrillMap::resize(isize w, isize h, isize d)
{
    assert(w >= MIN_MAP_WIDTH && w <= MAX_MAP_WIDTH);
    assert(h >= MIN_MAP_HEIGHT && h <= MAX_MAP_HEIGHT);

    width = w;
    height = h;
    depth = d;

    center = PrecisionComplex(Options::location.real, Options::location.imag);
    mpfPixelDeltaX = mpf_class(4.0) / Options::location.zoom / height;
    mpfPixelDeltaY = mpfPixelDeltaX;
    pixelDeltaX = mpfPixelDeltaY;
    pixelDeltaY = mpfPixelDeltaY;

    ul = translate(Coord());
    lr = translate(Coord(width - 1, height -1));

    resultMap.assign(width * height, DrillResult::DR_UNPROCESSED);
    firstIterationMap.assign(width * height, 0);
    lastIterationMap.assign(width * height, 0);
    overlayMap.assign(width * height, 0);
    textureMap.assign(width * height, 0);
    nitcntMap.assign(width * height, 0);
    distMap.assign(width * height, 0);
    derivReMap.assign(width * height, 0);
    derivImMap.assign(width * height, 0);
    normalReMap.assign(width * height, 0);
    normalImMap.assign(width * height, 0);

    assert(!hasIterations());
    assert(!hasNormalizedIterationCounts());
    assert(!hasDerivates());
    assert(!hasNormals());
}

void
DrillMap::set(isize w, isize h, const MapEntry &entry)
{
    assert(w < width && h < height);

    auto i = h * width + w;

    resultMap[i] = entry.result;
    firstIterationMap[i] = entry.first;
    lastIterationMap[i] = entry.last;
    derivReMap[i] = entry.derivative.re;
    derivImMap[i] = entry.derivative.im;
    normalReMap[i] = entry.normal.re;
    normalImMap[i] = entry.normal.im;

    auto znabs = entry.zn.abs();

    // Derive the normalized iteration count
    nitcntMap[i] = lastIterationMap[i] - std::log(std::log(znabs)) / std::log(2.0);

    // Derive the distance estimate
    distMap[i] = 2.0 * znabs * std::log(znabs) / entry.derivative.abs();

    // Declare all textures as being outdated
    dirty = true;
}

void
DrillMap::set(const struct Coord &c, const MapEntry &entry)
{
    set(c.x, c.y, entry);
}

PrecisionComplex
DrillMap::translate(const Coord &coord) const
{
    auto c = Coord(width / 2, height / 2);

    // Compute the pixel distance to the center
    auto dx = mpfPixelDeltaX * (coord.x - c.x);
    auto dy = mpfPixelDeltaY * (coord.y - c.y);

    return center + PrecisionComplex(dx, dy);
}

Coord
DrillMap::translate(const PrecisionComplex &coord) const
{
    auto c = Coord(width / 2, height / 2);

    // Compute the distance to the center
    auto dxy = coord -  center;
    mpf_class dx = dxy.re / mpfPixelDeltaX;
    mpf_class dy = dxy.re / mpfPixelDeltaX;

    return c + Coord(dx.get_si(), dy.get_si());
}

ExtendedComplex
DrillMap::distance(const Coord &coord, const Coord &other) const
{
    // Compute the pixel offset
    auto dx = coord.x - other.x;
    auto dy = coord.y - other.y;

    // Compute the delta location on the complex plain
    auto dxc = pixelDeltaX * dx;
    auto dyc = pixelDeltaY * dy;

    auto result = ExtendedComplex(dxc, dyc);
    result.reduce();

    return result;
}

ExtendedComplex
DrillMap::distance(const Coord &coord) const
{
    return distance(coord, Coord(width / 2, height / 2));
}

void
DrillMap::getMesh(isize numx, isize numy, std::vector<Coord> &mesh) const
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

bool
DrillMap::hasDrillResults() const
{
    for (isize i = 0; i < width * height; i++) {
        if (resultMap[i]) return true;
    }
    return false;
}

bool
DrillMap::hasIterations() const
{
    for (isize i = 0; i < width * height; i++) {
        if (lastIterationMap[i]) return true;
    }
    return false;
}

bool
DrillMap::hasNormalizedIterationCounts() const
{
    for (isize i = 0; i < width * height; i++) {
        if (nitcntMap[i]) return true;
    }
    return false;
}

bool
DrillMap::hasDistances() const
{
    for (isize i = 0; i < width * height; i++) {
        if (distMap[i]) return true;
    }
    return false;
}

bool
DrillMap::hasDerivates() const
{
    for (isize i = 0; i < width * height; i++) {
        if (derivReMap[i] || derivImMap[i]) return true;
    }
    return false;
}

bool
DrillMap::hasNormals() const
{
    for (isize i = 0; i < width * height; i++) {
        if (normalReMap[i] || normalImMap[i]) return true;
    }
    return false;
}

void
DrillMap::analyze() const
{
    struct {

        isize total = 0;
        isize interior = 0;
        isize exterior = 0;
        isize unprocessed = 0;
        isize glitches = 0;

    } spots;

    struct {

        isize total = 0;
        isize bulb = 0;
        isize cartioid = 0;
        isize approximations = 0;
        isize periods = 0;
        isize attractors = 0;

    } optspots;

    struct {

        isize total = 0;
        isize interior = 0;
        isize exterior = 0;

    } iterations;

    struct {

        isize total = 0;
        isize bulb = 0;
        isize cartioid = 0;
        isize approximations = 0;
        isize periods = 0;
        isize attractors = 0;

    } saved;

    auto total = width * height;
    auto limit = Options::location.depth;

    {   ProgressIndicator progress("Analyzing drill map", total);

        for (isize y = 0; y < height; y++) {
            for (isize x = 0; x < width; x++) {

                auto i = y * width + x;

                // auto &entry = get(Coord(x,y));

                bool optspot = !!firstIterationMap[i];
                optspots.approximations += !!firstIterationMap[i];
                saved.approximations += firstIterationMap[i];
                saved.total += firstIterationMap[i];

                switch(resultMap[i]) {

                    case DR_UNPROCESSED:

                        spots.total++;
                        spots.unprocessed++;
                        break;

                    case DR_ESCAPED:

                        spots.total++;
                        spots.exterior++;
                        iterations.total += lastIterationMap[i];
                        iterations.exterior += lastIterationMap[i];
                        break;

                    case DR_MAX_DEPTH_REACHED:

                        spots.total++;
                        spots.interior++;
                        iterations.total += lastIterationMap[i];
                        iterations.interior += lastIterationMap[i];
                        assert(lastIterationMap[i] == limit);
                        break;

                    case DR_IN_BULB:

                        optspot = true;
                        spots.total++;
                        spots.interior++;
                        optspots.bulb++;
                        iterations.total += limit;
                        iterations.interior += limit;
                        saved.total += limit;
                        saved.bulb += limit;
                        break;

                    case DR_IN_CARDIOID:

                        optspot = true;
                        spots.total++;
                        spots.interior++;
                        optspots.cartioid++;
                        iterations.total += limit;
                        iterations.interior += limit;
                        saved.total += limit;
                        saved.cartioid += limit;
                        break;

                    case DR_PERIODIC:

                        optspot = true;
                        spots.total++;
                        spots.interior++;
                        optspots.periods++;
                        iterations.total += limit;
                        iterations.interior += limit;
                        saved.total += limit - lastIterationMap[i];
                        saved.periods += limit - lastIterationMap[i];
                        break;

                    case DR_ATTRACTED:

                        optspot = true;
                        spots.total++;
                        spots.interior++;
                        optspots.attractors++;
                        iterations.total += limit;
                        iterations.interior += limit;
                        saved.total += limit - lastIterationMap[i];
                        saved.attractors += limit - lastIterationMap[i];
                        break;

                    case DR_GLITCH:

                        spots.total++;
                        spots.glitches++;
                        break;
                }

                if (optspot) optspots.total++;
            }
            progress.step(width);
        }
    }

    auto format = [&](isize x) {

        usize digits = std::log10(iterations.total) + 1;

        string result = std::to_string(x);
        result = std::string(digits - std::min(digits, result.length()), ' ') + result;

        auto p = isize(std::round(10000.0 * double(x) / double(total)));
        auto q1 = std::to_string(p / 100);
        auto q2 = std::to_string(p % 100);
        q1 = std::string(3 - std::min(usize(3), q1.length()), ' ') + q1;
        q2 = q2 + std::string(2 - std::min(usize(2), q2.length()), '0');

        result += " (" + q1 + "." + q2 + " %)";

        return result;
    };

    log::cout << log::vspace;
    log::cout << "           Drill locations: " << log::endl;
    log::cout << log::endl;
    log::cout << log::ralign("Total: ");
    log::cout << format(spots.total) << log::endl;
    log::cout << log::ralign("Unprocessed: ");
    log::cout << format(spots.unprocessed) << log::endl;
    log::cout << log::ralign("Interior: ");
    log::cout << format(spots.interior) << log::endl;
    log::cout << log::ralign("Exterior: ");
    log::cout << format(spots.exterior) << log::endl;
    log::cout << log::ralign("Glitches: ");
    log::cout << format(spots.glitches) << log::endl;

    log::cout << log::vspace;
    log::cout << "           Locations with applied optimizations: " << log::endl;
    log::cout << log::endl;
    log::cout << log::ralign("Total: ");
    log::cout << format(optspots.total) << log::endl;
    log::cout << log::ralign("Main bulb filter: ");
    log::cout << format(optspots.bulb) << log::endl;
    log::cout << log::ralign("Cartioid filter: ");
    log::cout << format(optspots.cartioid) << log::endl;
    log::cout << log::ralign("Series approximation: ");
    log::cout << format(optspots.approximations) << log::endl;
    log::cout << log::ralign("Period detection: ");
    log::cout << format(optspots.periods) << log::endl;
    log::cout << log::ralign("Attractor detection: ");
    log::cout << format(optspots.attractors) << log::endl;

    total = iterations.total;

    log::cout << log::vspace;
    log::cout << "           Iteration counts: " << log::endl;
    log::cout << log::endl;
    log::cout << log::ralign("Total: ");
    log::cout << format(iterations.total) << log::endl;
    log::cout << log::ralign("Interior: ");
    log::cout << format(iterations.interior) << log::endl;
    log::cout << log::ralign("Exterior: ");
    log::cout << format(iterations.exterior) << log::endl;

    log::cout << log::vspace;
    log::cout << "           Skipped iterations: " << log::endl;
    log::cout << log::endl;
    log::cout << log::ralign("Total: ");
    log::cout << format(saved.total) << log::endl;
    log::cout << log::ralign("Main bulb filter: ");
    log::cout << format(saved.bulb) << log::endl;
    log::cout << log::ralign("Cartioid filter: ");
    log::cout << format(saved.cartioid) << log::endl;
    log::cout << log::ralign("Series approximation: ");
    log::cout << format(saved.approximations) << log::endl;
    log::cout << log::ralign("Period detection: ");
    log::cout << format(saved.periods) << log::endl;
    log::cout << log::ralign("Attractor detection: ");
    log::cout << format(saved.attractors) << log::endl;
    log::cout << log::endl;
}

void
DrillMap::updateTextures()
{
    // Only proceed of textures are dirty
    if (!dirty) return;

    auto w = unsigned(width);
    auto h = unsigned(height);

    if (iterationMapTex.getSize() != sf::Vector2u(w, h)) {

        if (!iterationMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create iteration map texture");
        }
        if (!overlayMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create overlay map texture");
        }
        if (!nitcntMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create normalized iteration count map texture");
        }
        if (!distMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create distance map texture");
        }
        if (!normalReMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create normal(re) map texture");
        }
        if (!normalImMapTex.create(unsigned(width), unsigned(height))) {
            throw Exception("Can't create normal(im) map texture");
        }
    }

    // Generate the overlay image
    for (isize y = 0; y < height; y++) {
        for (isize x = 0; x < width; x++) {
            
            auto pos = y * width + x;
            
            switch (resultMap[pos]) {
                    
                case DR_ESCAPED:
                    
                    overlayMap[pos] = 0;
                    break;
                    
                case DR_GLITCH:
                    
                    overlayMap[pos] = Options::perturbation.color | 0xFF000000;
                    distMap[pos] = 0;
                    break;
                    
                case DR_IN_BULB:
                case DR_IN_CARDIOID:
                    
                    overlayMap[pos] = Options::areacheck.color | 0xFF000000;
                    distMap[pos] = 0;
                    break;
                    
                case DR_PERIODIC:
                    
                    overlayMap[pos] = Options::periodcheck.color | 0xFF000000;
                    distMap[pos] = 0;
                    break;
                    
                case DR_ATTRACTED:
                    
                    overlayMap[pos] = Options::attractorcheck.color | 0xFF000000;
                    distMap[pos] = 0;
                    break;
                    
                default:
                    
                    overlayMap[pos] = GpuColor::black | 0xFF000000;
                    distMap[pos] = 0;
                    break;
            }
        }
    }

    iterationMapTex.update((u8 *)lastIterationMap.data());
    overlayMapTex.update((u8 *)overlayMap.data());
    nitcntMapTex.update((u8 *)nitcntMap.data());
    distMapTex.update((u8 *)distMap.data());
    normalReMapTex.update((u8 *)normalReMap.data());
    normalImMapTex.update((u8 *)normalImMap.data());

    dirty = false;
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
    resize(width, height, depth);

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

    // Mark textures as outdated
    dirty = true;

    if (Options::flags.verbose) {

        log::cout << log::vspace;
        log::cout << log::ralign("Map size: ");
        log::cout << width << " x " << height << log::endl;
        log::cout << log::ralign("Drill results: ");
        log::cout << (hasDrillResults() ? "Loaded" : "Not included in map file") << log::endl;
        log::cout << log::ralign("Iteration counts: ");
        log::cout << (hasIterations() ? "Loaded" : "Not included in map file") << log::endl;
        log::cout << log::ralign("Normalized iteration counts: ");
        log::cout << (hasNormalizedIterationCounts() ? "Loaded" : "Not included in map file") << log::endl;
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

    // Read version number and map format
    u8 major;       is >> major;
    u8 minor;       is >> minor;
    u8 subminor;    is >> subminor;
    u8 beta;        is >> beta;
    u32 format;     is >> format;

    // Check check map format
    if (format != MAP_FORMAT) {
        throw Exception("The mapfile is incompatible with this release. It was generated with DeepDrill " +
                        Application::version(major, minor, subminor, beta) +
                        ".");
    }

    // Read map dimensions
    is.read((char *)&width, sizeof(width));
    is.read((char *)&height, sizeof(height));
    is.read((char *)&depth, sizeof(depth));
}

void
DrillMap::loadChannel(Compressor &is)
{
    u8 id;  is >> id;
    u8 fmt; is >> fmt;

    auto loadInt = [&]() {

        i64 value;

        switch (ChannelFormat(fmt)) {

            case FMT_I8:  load <FMT_I8>  (is, value); break;
            case FMT_I16: load <FMT_I16> (is, value); break;
            case FMT_I24: load <FMT_I24> (is, value); break;
            case FMT_I32: load <FMT_I32> (is, value); break;

            default:
                throw Exception("Invalid data format");
        }

        return value;
    };

    auto loadFloat = [&]() {

        double value;

        switch (ChannelFormat(fmt)) {

            case FMT_FP16: load <FMT_FP16> (is, value); break;
            case FMT_FLOAT: load <FMT_FLOAT> (is, value); break;
            case FMT_DOUBLE: load <FMT_DOUBLE> (is, value); break;

            default:
                throw Exception("Invalid data format");
        }

        return value;
    };

    switch (ChannelID(id)) {

        case CHANNEL_RESULT:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    resultMap[y * width + x] = DrillResult(loadInt());
                }
            }
            break;

        case CHANNEL_FIRST:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    firstIterationMap[y * width + x] = u32(loadInt());
                }
            }
            break;

        case CHANNEL_LAST:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    lastIterationMap[y * width + x] = u32(loadInt());
                }
            }
            break;

        case CHANNEL_NITCNT:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    nitcntMap[y * width + x] = float(loadFloat());
                }
            }
            break;

        case CHANNEL_DIST:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    distMap[y * width + x] = float(loadFloat());
                }
            }
            break;

        case CHANNEL_DERIVATIVE:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    derivReMap[y * width + x] = double(loadFloat());
                    derivImMap[y * width + x] = double(loadFloat());
                }
            }
            break;

        case CHANNEL_NORMAL:

            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    normalReMap[y * width + x] = float(loadFloat());
                    normalImMap[y * width + x] = float(loadFloat());
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
    bool saveFirst = true;
    bool saveLast = true;
    bool saveNrmItCnts = true;
    bool saveDist = true;
    bool saveDerivatives = false;
    bool saveNormals = Options::drillmap.depth == 1;

    Compressor compressor(width * height * sizeof(MapEntry));

    {   ProgressIndicator progress1("Preparing map file");

        // Write header
        saveHeader(os);

        // The next byte indicates if channel data is compressed
        os << u8(Options::drillmap.compress);

        // Generate channels
        if (saveDrillResults) saveChannel(compressor, CHANNEL_RESULT);
        if (saveFirst) saveChannel(compressor, CHANNEL_FIRST);
        if (saveLast) saveChannel(compressor, CHANNEL_LAST);
        if (saveNrmItCnts) saveChannel(compressor, CHANNEL_NITCNT);
        if (saveDist) saveChannel(compressor, CHANNEL_DIST);
        if (saveDerivatives) saveChannel(compressor, CHANNEL_DERIVATIVE);
        if (saveNormals) saveChannel(compressor, CHANNEL_NORMAL);
    }

    if (Options::flags.verbose) {

        log::cout << log::vspace;
        log::cout << log::ralign("Map size: ");
        log::cout << width << " x " << height << log::endl;
        log::cout << log::ralign("Drill results: ");
        log::cout << (saveDrillResults ? "Saved" : "Not saved") << log::endl;
        log::cout << log::ralign("Iteration counts: ");
        log::cout << (saveLast ? "Saved" : "Not saved") << log::endl;
        log::cout << log::ralign("Skipped interations: ");
        log::cout << (saveFirst ? "Saved" : "Not saved") << log::endl;
        log::cout << log::ralign("Normalized iteration counts: ");
        log::cout << (saveNrmItCnts ? "Saved" : "Not saved") << log::endl;
        log::cout << log::ralign("Derivatives: ");
        log::cout << (saveDerivatives ? "Saved" : "Not saved") << log::endl;
        log::cout << log::ralign("Normals: ");
        log::cout << (saveNormals ? "Saved" : "Not saved") << log::endl;
        log::cout << log::vspace;
    }

    if (Options::drillmap.compress) {

        ProgressIndicator progress2("Compressing map file");

        auto oldSize = compressor.size();
        compressor.compressData();
        auto newSize = compressor.size();
        auto saved = oldSize - newSize;
        progress2.done();

        if (Options::flags.verbose) {

            log::cout << log::vspace;
            log::cout << log::ralign("Size reduction: ");
            log::cout << saved << " Bytes (" << isize(100.0 * saved / oldSize) << "%)" << log::endl;
            log::cout << log::vspace;
        }
    }

    ProgressIndicator progress3("Saving map file");
    compressor >> os;
}

void
DrillMap::saveHeader(std::ostream &os)
{
    // Write magic bytes
    os.write("DeepDrill", 9);

    // Write version number and map format
    os << u8(VER_MAJOR);
    os << u8(VER_MINOR);
    os << u8(VER_SUBMINOR);
    os << u8(VER_BETA);
    os << u32(MAP_FORMAT);

    // Write map dimensions
    os.write((char *)&width, sizeof(width));
    os.write((char *)&height, sizeof(height));
    os.write((char *)&depth, sizeof(depth));
}

void
DrillMap::saveChannel(Compressor &os, ChannelID id)
{
    switch (id) {

        case CHANNEL_RESULT:

            os << u8(id) << u8(FMT_I8);
            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    save <FMT_I8> (os, i8(resultMap[y * width + x]));
                }
            }
            break;

        case CHANNEL_FIRST:

            os << u8(id) << u8(FMT_I32);
            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    save <FMT_I32> (os, firstIterationMap[y * width + x]);
                }
            }
            break;

        case CHANNEL_LAST:

            os << u8(id) << u8(FMT_I32);
            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    save <FMT_I32> (os, lastIterationMap[y * width + x]);
                }
            }
            break;

        case CHANNEL_NITCNT:

            os << u8(id) << u8(FMT_FLOAT);
            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    save <FMT_FLOAT> (os, nitcntMap[y * width + x]);
                }
            }
            break;

        case CHANNEL_DIST:

            os << u8(id) << u8(FMT_FLOAT);
            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    save <FMT_FLOAT> (os, distMap[y * width + x]);
                }
            }
            break;

        case CHANNEL_DERIVATIVE:

            os << u8(id) << u8(FMT_FLOAT);
            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    save <FMT_FLOAT> (os, derivReMap[y * width + x]);
                    save <FMT_FLOAT> (os, derivImMap[y * width + x]);
                }
            }
            break;

        case CHANNEL_NORMAL:

            os << u8(id) << u8(FMT_FP16);
            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    save <FMT_FP16> (os, normalReMap[y * width + x]);
                    save <FMT_FP16> (os, normalImMap[y * width + x]);
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

}
