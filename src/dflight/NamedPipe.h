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
// #include "commons.h"

namespace dd {

struct NamedPipe {

    int pipe = -1;
    string name;

public:

    bool create();
    bool create(const string &name);
    bool open();
    bool isOpen();
    bool close();
    isize write(u8 *buffer, isize length);
};

}

