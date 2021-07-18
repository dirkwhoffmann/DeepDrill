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

using namespace dd;

int main(int argc, char *argv[])
{
    std::vector<string> args;

    for (int i = 0; i < argc; i++) {
        args.push_back(string(argv[i]));
    }
        
    try {

        dd::Application().main(args);

    } catch (dd::SyntaxError &e) {
                
        log::cout << "Syntax: ";
        // log::cout << dd::extractName(string(argv[0]));
        log::cout << "deepdrill";
        log::cout << " [-v] [-p <profile>] -o <output> <input>";
        log::cout << log::endl;
        
        if (!e.description.empty()) {
            log::cout << e.what() << log::endl;
        }
        return 1;

    } catch (std::exception &e) {
        
        log::cout << e.what() << log::endl;
        return 1;
    }
    
    return 0;
}
