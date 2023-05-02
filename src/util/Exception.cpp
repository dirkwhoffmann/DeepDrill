// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "Exception.h"
#include "Logger.h"

namespace dd {

void
Exception::what(const class Logger &log) const
{
    log::cout << log::red << log::bold << "Error: ";
    log::cout << what() << log::light << log::normal;
}

void
UserInterruptException::what(const class Logger &log) const
{
    log::cout << log::purple << log::bold << "User Interrupt";
    log::cout << log::light << log::normal;
}

void
ParseError::what(const class Logger &log) const
{
    log::cout << log::red << log::bold;
    log::cout << "Error in file " << file << ", line " << line << ": ";
    log::cout << exception.what() << log::light << log::normal;
}

}
