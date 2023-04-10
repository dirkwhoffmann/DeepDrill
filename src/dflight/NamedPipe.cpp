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

#include "NamedPipe.h"

#include <filesystem>
#include <thread>

namespace dd {

bool
NamedPipe::create()
{
    std::ostringstream ss;
    auto id = std::this_thread::get_id();
    auto dir = std::filesystem::temp_directory_path();

    ss << id;
    auto file = dir / ss.str();

    return create(file.string());
}

bool
NamedPipe::create(const string &name)
{
    this->name = name;
    ::unlink(name.c_str());
    return ::mkfifo(name.c_str(), 0666) != -1;
}

bool
NamedPipe::open()
{
    pipe = ::open(name.c_str(), O_WRONLY);
    return pipe != -1;
}

bool
NamedPipe::isOpen()
{
    return pipe != -1;
}

bool
NamedPipe::close()
{
    auto result = ::close(pipe);
    pipe = -1;
    return result == 0;
}

isize
NamedPipe::write(u8 *buffer, isize length)
{
    assert(isOpen());
    return ::write(pipe, (void *)buffer, (size_t)length);
}

}
