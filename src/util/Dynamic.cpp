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

#include "Dynamic.h"
#include "spline.h"

namespace dd {

template <class T> double
Dynamic<T>::fps = 60.0;

template <class T> void
Dynamic<T>::init(std::vector <T> vxn, std::vector <T> vyn)
{
    for (const auto &it : vxn) xn.push_back(double(it));
    for (const auto &it : vyn) yn.push_back(double(it));

    if (xn.size() >= 3) {
        
        spline = tk::spline(xn, yn, tk::spline::cspline, true);
    }
}

template <class T> std::ostream&
Dynamic<T>::print(std::ostream& os) const
{
    if (yn.size() == 1) {

        os << (T)yn[0];

    } else {

        for (usize i = 0; i < xn.size(); i++) {

            if (i) os << ", ";
            os << (T)xn[i] << '/' << (T)yn[i];
        }
    }

    return os;
}

template <class T> T
Dynamic<T>::operator() (double x) const
{
    return yn.size() == 1 ? (T)yn[0] : (T)spline(x);
}

template <class T> T
Dynamic<T>::operator() (isize x) const
{
    return (*this)(double(x) / fps);
}

//
// Instantiate template functions
//

template void Dynamic<float>::init(std::vector <float>, std::vector <float>);
template float Dynamic<float>::operator() (double) const;
template float Dynamic<float>::operator() (isize) const;
template std::ostream& Dynamic<float>::print(std::ostream &) const;

}

