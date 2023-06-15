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

Compressor::Compressor()
{

}

Compressor&
Compressor::operator<<(const u8 &value)
{
    uncompressed.push_back(value);

    return *this;
}

Compressor&
Compressor::operator<<(const u16 &value)
{
    uncompressed.push_back(u8(value >> 0));
    uncompressed.push_back(u8(value >> 8));

    return *this;
}

Compressor&
Compressor::operator<<(const u32 &value)
{
    uncompressed.push_back(u8(value >> 0));
    uncompressed.push_back(u8(value >> 8));
    uncompressed.push_back(u8(value >> 16));
    uncompressed.push_back(u8(value >> 24));

    return *this;
}

Compressor&
Compressor::operator<<(const float &value)
{
    u8 *p = (u8 *)&value;

    for (isize i = 0; i < isizeof(float); i++) {
        uncompressed.push_back(p[i]);
    }
    return *this;
}

Compressor&
Compressor::operator<<(const double &value)
{
    u8 *p = (u8 *)&value;

    for (isize i = 0; i < isizeof(double); i++) {
        uncompressed.push_back(p[i]);
    }
    return *this;
}

Compressor&
Compressor::operator<<(std::istream &is)
{
    // Adjust the buffer size
    is.seekg(0, std::ios::end);
    std::streamsize size = is.tellg();
    is.seekg(0, std::ios::beg);
    uncompressed.resize(size);

    // Read data
    if (!is.read((char *)uncompressed.data(), size)) {
        throw Exception("Compressor: Can't read from stream");
    }

    return *this;
}

Compressor&
Compressor::operator>>(std::ostream &os)
{
    // Start with a big enough buffer
    compressed.resize(uncompressed.size());

    auto srcLen = mz_ulong(uncompressed.size());
    auto dstLen = mz_ulong(compressed.size());

    // Add some bytes as the size might grow slightly in rare situations
    dstLen += 128;

    // Run the compressor
    auto result = mz_compress(compressed.data(), &dstLen, uncompressed.data(), srcLen);

    if (result != MZ_OK) {
        throw Exception("Compressor: mz_compress failed with error code " + std::to_string(result));
    }

    printf("After compression (%d): %lu bytes\n", result, dstLen);

    // Write compressed data to stream
    os.write((char *)compressed.data(), dstLen);

    return *this;
}

Uncompressor::Uncompressor(isize capacity)
{
    uncompressed.resize(capacity);
}

Uncompressor&
Uncompressor::operator<<(std::istream &is)
{
    // Adjust the buffer size
    is.seekg(0, std::ios::end);
    std::streamsize size = is.tellg();
    is.seekg(0, std::ios::beg);
    compressed.resize(size);

    // Read data
    if (!is.read((char *)compressed.data(), size)) {
        throw Exception("Uncompressor: Can't read from stream");
    }

    return *this;
}

Uncompressor&
Uncompressor::operator>>(std::ostream &os)
{
    auto srcLen = mz_ulong(compressed.size());
    auto dstLen = mz_ulong(uncompressed.size());

    // Run the uncompressor
    auto result = mz_uncompress(uncompressed.data(), &dstLen, compressed.data(), srcLen);

    if (result != MZ_OK) {
        throw Exception("Uncompressor: mz_uncompress failed with error code " + std::to_string(result));
    }

    printf("After uncompression: %lu bytes\n", dstLen);

    // Write uncompressed data to stream
    os.write((char *)uncompressed.data(), dstLen);

    return *this;
}

}
