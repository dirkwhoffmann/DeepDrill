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

#include "config.h"
#include "Application.h"
#include "Options.h"
#include "UnitTester.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>

namespace dd {

int
Application::main(std::vector <string> &args)
{
    map<string,string> keys;
    
    std::cout << "Deep Drill " << VER_MAJOR << "." << VER_MINOR;
    std::cout << " - (C)opyright 2021 Dirk W. Hoffmann";
    std::cout << std::endl << std::endl;
        
    // Run a self-test
    if (!releaseBuild) UnitTester().run();
    
    try {
        
        // Read all config files
        for (auto &arg: args) readConfigFile(arg, keys);
        
        // Setup the GMP library
        setupGmp(keys);
        
        // Parse all options
        Options options(keys);
        
        // Create a driller
        Driller driller(options);
        driller.drill();
        
    } catch(boost::exception const& e) {
        std::cout << dynamic_cast<std::exception const&>(e).what() << std::endl;
        return 1;
        
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void
Application::readConfigFile(string path, std::map <string,string> &keys)
{
    boost::property_tree::ptree tree;

    // Create a property tree
    boost::property_tree::ini_parser::read_ini(path, tree);
                
    // Scan all keys
    for (auto &it1: tree) {
        
        auto key1 = it1.first;
        std::transform(key1.begin(), key1.end(), key1.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        
        for (auto &it2: it1.second) {

            auto key2 = it2.first;
            std::transform(key2.begin(), key2.end(), key2.begin(),
                           [](unsigned char c){ return std::tolower(c); });

            keys.insert(std::make_pair(key1 + "." + key2, it2.second.data()));
        }
    }    
}

void
Application::setupGmp(std::map <string,string> &keys)
{
    long exponent = 0;
    
    // Peek the zoom factor
    if (auto it = keys.find("location.zoom"); it != keys.end()) {
        
        try {
            auto zoom = mpf_class(it->second);
            mpf_get_d_2exp(&exponent, zoom.get_mpf_t());
        } catch (...) { }
        
        mpf_set_default_prec(exponent + 64);
    }
}

}
