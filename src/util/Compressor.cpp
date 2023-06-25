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

#include "Compressor.h"
#include "Exception.h"
#include "miniz.h"

namespace dd {

bool
Compressor::eof()
{
    return ptr == isize(buffer.size());
}

Compressor&
Compressor::operator<<(std::istream &is)
{
    // Determine the number of bytes to read
    std::streamsize current = is.tellg();
    is.seekg(0, std::ios::end);
    std::streamsize last = is.tellg();
    is.seekg(current);
    auto size = last - current;

    // Read data
    buffer.resize(size);
    if (!is.read((char *)buffer.data(), size)) {
        throw Exception("Compressor: Can't read from stream");
    }

    return *this;
}

Compressor&
Compressor::operator>>(std::ostream &os)
{
    os.write((char *)buffer.data(), buffer.size());
    return *this;
}

void
Compressor::compressData()
{
    // Create a second buffer
    std::vector<u8> target(capacity);

    auto bufferLen = mz_ulong(buffer.size());
    auto targetLen = mz_ulong(target.size());

    // Run the compressor
    auto result = mz_compress(target.data(), &targetLen, buffer.data(), bufferLen);
    if (result != MZ_OK) {
        throw Exception("Compression failed with error code " + std::to_string(result));
    }

    // Resize the target buffer to it's actual size
    target.resize(targetLen);

    // Replace original data with compressed data
    buffer.swap(target);
}

void
Compressor::uncompressData()
{
    // Create a second buffer
    std::vector<u8> target(capacity);

    auto bufferLen = mz_ulong(buffer.size());
    auto targetLen = mz_ulong(target.size());

    // Run the uncompressor
    auto result = mz_uncompress(target.data(), &targetLen, buffer.data(), bufferLen);
    if (result != MZ_OK) {
        throw Exception("Uncompression failed with error code " + std::to_string(result));
    }

    // Resize the target buffer to it's actual size
    target.resize(targetLen);

    // Replace original data with compressed data
    buffer.swap(target);
}

}
