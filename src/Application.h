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

#include "commons.h"
#include "DrillMap.h"

namespace dd {

class Application {
        
    // Extracted command line arguments
    std::vector <string> profiles;
    std::vector <string> inputs;
    std::vector <string> outputs;
    
    // The drill map
    DrillMap drillMap;
    
public:
        
    // Main entry point
    void main(std::vector <string> &args) throws;
    
private:
    
    // Helper methods for parsing command line arguments
    void parseArguments(std::vector <string> &args, map<string,string> &keys) throws;
    void parseOption(vector <string> &args, map<string,string> &keys) throws;
    string pop(vector <string> &args) throws;

    void checkArguments(map<string,string> &keys) throws;

    void readInputs(map<string,string> &keys) throws;
    void readOutputs(map<string,string> &keys) throws;
    void readProfiles(map<string,string> &keys) throws;
            
    // Sets up the GMP library
    void setupGmp(std::map <string,string> &keys);
};

}
