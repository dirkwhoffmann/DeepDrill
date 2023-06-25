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
    
    isize capacity = 0;
    isize ptr = 0;

    std::vector <u8> buffer;

public:

    Compressor(isize capacity) : capacity(capacity) { }
    ~Compressor() { };

    bool eof();
    isize size() { return (isize)buffer.size(); }

    Compressor& operator<<(const i8 &value) { write(value); return *this; }
    Compressor& operator<<(const u8 &value) { write(value); return *this; }
    Compressor& operator<<(const i16 &value) { write(value); return *this; }
    Compressor& operator<<(const u16 &value) { write(value); return *this; }
    Compressor& operator<<(const i32 &value) { write(value); return *this; }
    Compressor& operator<<(const u32 &value) { write(value); return *this; }
    Compressor& operator<<(const float &value) { write(value); return *this; }
    Compressor& operator<<(const double &value) { write(value); return *this; }
    Compressor& operator<<(std::istream &is);

    Compressor& operator>>(i8 &value) { read(value); return *this; }
    Compressor& operator>>(u8 &value) { read(value); return *this; }
    Compressor& operator>>(i16 &value) { read(value); return *this; }
    Compressor& operator>>(u16 &value) { read(value); return *this; }
    Compressor& operator>>(i32 &value) { read(value); return *this; }
    Compressor& operator>>(u32 &value) { read(value); return *this; }
    Compressor& operator>>(float &value) { read(value); return *this; }
    Compressor& operator>>(double &value) { read(value); return *this; }
    Compressor& operator>>(std::ostream &os);

    void compressData();
    void uncompressData();

private:

    template <typename T> void write(T value) {

        u8 *p = (u8 *)&value;

        for (isize i = 0; i < isizeof(T); i++) {
            buffer.push_back(p[i]);
        }
    }
    
    template <typename T> void read(T &value) {

        u8 *p = (u8 *)&value;

        for (isize i = 0; i < isizeof(T); i++) {
            p[i] = buffer[ptr++];
        }
    }
};

}
