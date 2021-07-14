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
        
    try {

        dd::Application().main(args);

    } catch (dd::SyntaxError &e) {
        
        std::cout << "Syntax: " << string(argv[0]);
        std::cout << " [-v] [-p <profile.prf>] [-o <mapfile.map>] <location.loc>";
        std::cout << std::endl;
        return 1;
    
    } catch (std::exception &e) {
        
        std::cout << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
