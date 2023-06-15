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

class Compressor {

    std::vector<u8> uncompressed;
    std::vector<u8> compressed;

public:

    Compressor();
    ~Compressor() { };

    Compressor& operator<<(const u8 &value);
    Compressor& operator<<(const u16 &value);
    Compressor& operator<<(const u32 &value);
    Compressor& operator<<(const float &value);
    Compressor& operator<<(const double &value);
    Compressor& operator<<(std::istream &is);
    Compressor& operator>>(std::ostream &os);
};

class Uncompressor {

    std::vector<u8> uncompressed;
    std::vector<u8> compressed;

public:

    Uncompressor(isize capacity);
    ~Uncompressor() { };

    Uncompressor& operator<<(std::istream &is);
    Uncompressor& operator>>(std::ostream &os);
};
}

