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

#include "MathTypes.h"
#include "Exception.h"

namespace dd {

class Application {
        
public:
        
    // Main entry point
    void main(std::vector <string> &args) throws;
    
private:
    
    // Helper methods for parsing command line arguments
    bool isOption(const string &s);
    bool isProfile(const string &s);
    void parseOption(vector <string> &args, map<string,string> &keys) throws;
    void parseProfile(vector <string> &args, map<string,string> &keys) throws;

    // Reads in the location file
    void readLocationFile(map<string,string> &keys) throws;

    // Sets up the GMP library
    void setupGmp(std::map <string,string> &keys);
};

}
