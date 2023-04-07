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
#include "FFmpeg.h"
#include "NamedPipe.h"
#include "Buffer.h"

namespace dd {

class Recorder {

    // Configuration options
    const struct Options &opt;


    //
    // Handles
    //

    // FFmpeg instances
    FFmpeg videoFFmpeg;

    // Video and audio pipes
    NamedPipe videoPipe;


    //
    // Recording status
    //

    // All possible recorder states
    enum class State { wait, prepare, record, finalize, abort };

    // The current recorder state
    State state = State::wait;


    //
    // Recording parameters
    //

    // Video properties
    isize frameRate = 0;
    isize bitRate = 0;
    isize sampleRate = 0;

    // Time stamps
    Time recStart;
    Time recStop;

    // Temporary data storage
    Buffer<u32> videoData;


    //
    // Initializing
    //

public:

    Recorder(Options &opt);


    //
    // Querying locations and flags
    //

    // Returns the paths to the two named input pipes
    string videoPipePath();

    // Return the paths to the two temporary output files
    string videoStreamPath();

    //Returns the log level passed to FFmpeg
    // const string loglevel() { return REC_DEBUG ? "verbose" : "warning"; }
    const string loglevel() { return "warning"; }


    //
    // Querying recording parameters
    //

public:

    Time getDuration() const;
    isize getFrameRate() const { return frameRate; }
    isize getBitRate() const { return bitRate; }
    isize getSampleRate() const { return sampleRate; }


    //
    // Starting and stopping a video capture
    //

public:

    // Checks whether the screen is currently recorded
    bool isRecording() const { return state != State::wait; }

    // Starts the screen recorder
    void startRecording(isize bitRate, isize aspectX, isize aspectY);

    // Stops the screen recorder
    void stopRecording();

    // Exports the recorded video
    bool exportAs(const string &path);


    //
    // Recording a video stream
    //

public:

private:

    void prepare();
    void record();
    void recordVideo();
    void finalize();
    void abort();
};

}
