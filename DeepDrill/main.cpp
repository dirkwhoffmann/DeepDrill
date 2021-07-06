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

#include "Application.h"
#include <iostream>

int main(int argc, char *argv[])
{
    std::vector<string> args;

    for (int i = 1; i < argc; i++) {
        args.push_back(string(argv[i]));
    }
    
    if (args.empty()) {
        std::cout << "Syntax: " << string(argv[0]) << " config1.ini [config2.ini ...]";
        std::cout << std::endl;
        return 1;
    }
    
    dd::Application().main(args);
}
