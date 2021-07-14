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

#pragma once

#include "Types.h"
#include "Exception.h"

namespace dd {

class Application {
        
public:
        
    // Main entry point
    void main(std::vector <string> &args) throws;
    
private:
    
    // Helper methods for parsing command line arguments
    void parseArguments(std::vector <string> &args, map<string,string> &keys) throws;
    void parseOption(vector <string> &args, map<string,string> &keys) throws;
    void parseMapFile(vector <string> &args, map<string,string> &keys) throws;
    string pop(vector <string> &args) throws;    
};

}

