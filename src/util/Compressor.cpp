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
    printf("ptr = %zd size = %zu\n", ptr, buffer.size());
    return ptr == isize(buffer.size());
}

Compressor&
Compressor::operator<<(std::istream &is)
{
    // Determine the number of bytes to read
    std::streamsize current = is.tellg();
    printf("Current position = %ld\n", current);
    is.seekg(0, std::ios::end);
    std::streamsize last = is.tellg();
    printf("Last position = %ld\n", last);
    is.seekg(current);
    auto size = last - current;
    printf("Reading in %ld bytes\n", size);

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
    std::vector <u8> target(capacity);
    printf("Capacity of seconf buffer: %zu\n", target.size());

    auto bufferLen = mz_ulong(buffer.size());
    auto targetLen = mz_ulong(target.size());

    // Run the compressor
    auto result = mz_compress(target.data(), &targetLen, buffer.data(), bufferLen);
    if (result != MZ_OK) {
        throw Exception("Compression failed with error code " + std::to_string(result));
    }

    printf("After compression: %lu bytes\n", targetLen);

    // Resize the target buffer to it's actual size
    target.resize(targetLen);

    // Replace original data with compressed data
    buffer.swap(target);

    printf("Buffer length is now %zu bytes\n", buffer.size());

}

void
Compressor::uncompressData()
{
    // Create a second buffer
    std::vector <u8> target(capacity);
    printf("Uncompress: Capacity of second buffer: %zu\n", target.size());

    auto bufferLen = mz_ulong(buffer.size());
    auto targetLen = mz_ulong(target.size());

    // Run the uncompressor
    auto result = mz_uncompress(target.data(), &targetLen, buffer.data(), bufferLen);
    if (result != MZ_OK) {
        throw Exception("Uncompression failed with error code " + std::to_string(result));
    }

    printf("After uncompression: %lu bytes\n", targetLen);

    // Resize the target buffer to it's actual size
    target.resize(targetLen);

    // Replace original data with compressed data
    buffer.swap(target);

    printf("Buffer length is now %zu bytes\n", buffer.size());
}


/*
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
*/

}
