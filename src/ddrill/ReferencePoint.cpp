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
#include "DrillMap.h"
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

}
