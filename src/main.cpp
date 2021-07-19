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
#include "Logger.h"

using namespace dd;

int main(int argc, char *argv[])
{
    try {
        
        Application().main(argc, argv);
        
    } catch (dd::SyntaxError &e) {
        
        log::cout << "Usage: ";
        log::cout << "deepdrill [-bmv] [-p <profile>] -o <output> <input>" << log::endl;
        log::cout << log::endl;
        log::cout << "       -b or --batch    Run in batch mode" << log::endl;
        log::cout << "       -m or --make     Run the Makefile generator" << log::endl;
        log::cout << "       -v or --verbose  Run in verbose mode" << log::endl;
        log::cout << "       -p or --profile  Customize settings" << log::endl;
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
