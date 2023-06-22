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
#include "PrecisionComplex.h"
#include "StandardComplex.h"

namespace dd {

ReferenceIteration::ReferenceIteration(PrecisionComplex z, double tolerance)
{
    auto tt = tolerance * tolerance;
    
    this->standard = z;

    this->extended = z;
    this->extended.reduce();

    this->extended2 = extended * 2;
    this->extended2.reduce();

    this->tolerance = tt * standard.norm();
}

ReferenceIteration::ReferenceIteration(PrecisionComplex z, PrecisionComplex dz, double tolerance)
: ReferenceIteration(z, tolerance)
{
    this->derivative = dz;
}

ReferencePoint::ReferencePoint(Coord c, const PrecisionComplex &pc)
{
    this->coord = c;
    this->location = pc;
}

/*
ReferencePoint::ReferencePoint(const Options &opt, Coord c)
{
    this->coord = c;
    this->location = c.translate(opt);
}
*/

ExtendedComplex
ReferencePoint::deltaLocation(const Options &opt, const Coord &other) const
{
    // Compute the pixel offset
    auto dx = other.x - coord.x;
    auto dy = other.y - coord.y;
        
    // Compute the delta location on the complex plain
    auto dxc = opt.pixelDeltaX * dx;
    auto dyc = opt.pixelDeltaY * dy;
     
    auto result = ExtendedComplex(dxc, dyc);
    result.reduce();
    
    return result;
}

}
