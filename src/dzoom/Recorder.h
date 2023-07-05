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

#include "config.h"
#include "Types.h"
#include "FFmpeg.h"
#include "NamedPipe.h"

#include <SFML/Graphics.hpp>

namespace dd {

class Recorder {

    //
    // Handles
    //

    // FFmpeg instances
    FFmpeg videoFFmpeg;

    // Video and audio pipes
    NamedPipe videoPipe;


    //
    // Recording parameters
    //
    
    // Temporary data storage
    std::vector <u32> videoData;


    //
    // Initializing
    //

public:

    Recorder() { };


    //
    // Querying
    //

    //Returns the log level passed to FFmpeg
    const string loglevel() { return REC_DEBUG ? "verbose" : "warning"; }


    //
    // Starting and stopping
    //

public:

    // Starts the screen recorder
    void startRecording();

    // Stops the screen recorder
    void stopRecording();


    //
    // Recording
    //

public:

    void record(const sf::Image &img);
};

}
