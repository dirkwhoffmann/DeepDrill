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

#include "UnitTester.h"
#include <iostream>
#include <iomanip>

namespace dd {

UnitTester::UnitTester()
{
    srand(42);
}

void
UnitTester::run()
{
    std::cout << std::right << std::setw(30) << "Self check: ";

    for (isize i = 0; i < 1000; i++) {
        
        double re1 = (double)rand() / RAND_MAX;
        double im1 = (double)rand() / RAND_MAX;
        long ex1 = rand() % 8;

        auto std1 = StandardComplex(re1,im1) * pow(2,ex1);
        auto ext1 = ExtendedComplex( StandardComplex {re1,im1}, ex1);

        double re2 = (double)rand() / RAND_MAX;
        double im2 = (double)rand() / RAND_MAX;
        long ex2 = rand() % 8;

        auto std2 = StandardComplex(re2,im2) * pow(2,ex2);
        auto ext2 = ExtendedComplex( StandardComplex {re2,im2}, ex2);

        assertAlmostEqual(std1, ext1.asStandardComplex());
        assertAlmostEqual(std2, ext2.asStandardComplex());

        auto std = std1;
        auto ext = ext1;
        
        std += std2;
        ext += ext2;
        assertAlmostEqual(std, ext.asStandardComplex());

        std = std1 + std2;
        ext = ext1 + ext2;
        assertAlmostEqual(std, ext.asStandardComplex());

        std -= std2;
        ext -= ext2;
        assertAlmostEqual(std, ext.asStandardComplex());

        std = std1 - std2;
        ext = ext1 - ext2;
        assertAlmostEqual(std, ext.asStandardComplex());

        std *= std2;
        ext *= ext2;
        assertAlmostEqual(std, ext.asStandardComplex());

        std = std1 * std2;
        ext = ext1 * ext2;
        assertAlmostEqual(std, ext.asStandardComplex());

        std /= std2;
        ext /= ext2;
        assertAlmostEqual(std, ext.asStandardComplex());

        std = std1 / std2;
        ext = ext1 / ext2;
        assertAlmostEqual(std, ext.asStandardComplex());

        std = std1.square();
        ext = ext1.square();
        assertAlmostEqual(std, ext.asStandardComplex());

        std = std1.conjugate();
        ext = ext1.conjugate();
        assertAlmostEqual(std, ext.asStandardComplex());

        std = std1.reciprocal();
        ext = ext1.reciprocal();
        assertAlmostEqual(std, ext.asStandardComplex());

        auto stdnrm = std1.norm();
        auto extnrm = ext1.norm();
        assertAlmostEqual(stdnrm, extnrm.asDouble());
    }
    
    std::cout << "PASS" << std::endl;
}

void
UnitTester::assertAlmostEqual(const double x, const double y)
{
    if (std::abs(x - y) > 1e-12) {
        std::cout << "ERROR: " << x << " != " << y << std::endl;
        assert(false);
    }
}

void
UnitTester::assertAlmostEqual(const StandardComplex &x, const StandardComplex &y)
{
    // std::cout << "x: " << x << " y: " << y << std::endl;
    
    if (std::abs(x.re - y.re) > 1e-12) {
        std::cout << "ERROR (Re): " << x << " != " << y << std::endl;
        assert(false);
    }
    if (std::abs(x.im - y.im) > 1e-12) {
        std::cout << "ERROR (Im): " << x << " != " << y << std::endl;
        assert(false);
    }
}

}
