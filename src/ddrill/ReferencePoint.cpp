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

#include "ReferencePoint.h"
#include "Options.h"
#include "ProgressIndicator.h"

namespace dd {

ReferenceIteration::ReferenceIteration(PrecisionComplex c, double tolerance)
{
    auto tt = tolerance * tolerance;
    
    this->standard = c;

    this->extended = c;
    this->extended.reduce();

    this->extended2 = extended * 2;
    this->extended2.reduce();

    this->tolerance = tt * standard.norm();
}

ReferencePoint::ReferencePoint(const Options &opt, Coord c)
{
    this->coord = c;
    this->location = c.translate(opt);
}

ExtendedComplex
ReferencePoint::deltaLocation(const Options &opt, const Coord &other) const
{
    // Compute the pixel offset
    auto dx = other.x - coord.x;
    auto dy = other.y - coord.y;
        
    // Compute the delta location on the complex plain
    auto dxc = opt.pixelDelta * dx;
    auto dyc = opt.pixelDelta * dy;
     
    auto result = ExtendedComplex(dxc, dyc);
    result.reduce();
    
    return result;
}

}
