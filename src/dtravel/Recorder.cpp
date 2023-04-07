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

#include "Recorder.h"
#include "Options.h"
#include "Logger.h"

namespace dd {

Recorder::Recorder(Options &opt) : opt(opt)
{
    FFmpeg::init();
}

string
Recorder::videoPipePath()
{
    return "/tmp/videoPipe";
}

string
Recorder::videoStreamPath()
{
    return "/tmp/video.mp4";
}

Time
Recorder::getDuration() const
{
    return (isRecording() ? Time::now() : recStop) - recStart;
}

void
Recorder::startRecording(isize bitRate, isize aspectX, isize aspectY)
{
    if (isRecording()) {
        throw Exception("Recording in progress.");
    }

    // Create pipe
    if (opt.verbose) log::cout << log::vspace << "Creating pipes..." << log::endl;

    if (!videoPipe.create(videoPipePath())) {
        throw Exception("Failed to create the video encoder pipe.");
    }

    // Set the bit rate, frame rate, and sample rate
    this->bitRate = bitRate;
    frameRate = 60;
    sampleRate = 44100;

    // Create temporary buffers
    if (opt.verbose) log::cout << "Creating buffers..." << log::endl;
    videoData.alloc(opt.image.width * opt.image.height);

    //
    // Assemble the command line arguments for the video encoder
    //

    if (opt.verbose) log::cout << "Assembling command line arguments..." << log::endl;

    // Console interactions
    string cmd1 = " -nostdin";

    // Verbosity
    cmd1 += " -loglevel " + loglevel();

    // Input stream format
    cmd1 += " -f:v rawvideo -pixel_format rgba";

    // Frame rate
    cmd1 += " -r " + std::to_string(frameRate);

    // Frame size (width x height)
    cmd1 += " -s:v " + std::to_string(opt.image.width) + "x" + std::to_string(opt.image.height);

    // Input source (named pipe)
    cmd1 += " -i " + videoPipePath();

    // Output stream format
    cmd1 += " -f mp4 -pix_fmt yuv420p";

    // Bit rate
    cmd1 += " -b:v " + std::to_string(bitRate) + "k";

    // Aspect ratio
    cmd1 += " -bsf:v ";
    cmd1 += "\"h264_metadata=sample_aspect_ratio=";
    cmd1 += std::to_string(aspectX) + "/" + std::to_string(2*aspectY) + "\"";

    // Output file
    cmd1 += " -y " + videoStreamPath();

    //
    // Launch FFmpeg instance
    //

    assert(!videoFFmpeg.isRunning());

    // Launch the video encoder
    if (!videoFFmpeg.launch(cmd1)) {
        throw Exception("Unable to launch the FFmpeg encoder.");
    }

    // Open the video pipe
    if (opt.verbose) log::cout << "Opening video pipe..." << log::endl;

    if (!videoPipe.open()) {
        throw Exception("Unable to open the video pipe.");
    }


    if (opt.verbose) log::cout << "Success." << log::endl;
    state = State::prepare;
}

void
Recorder::stopRecording()
{
    if (isRecording()) {
        state = State::finalize;
    }
}

bool
Recorder::exportAs(const string &path)
{
    return true;
}

void
Recorder::prepare()
{
    state = State::record;
    recStart = Time::now();
}

void
Recorder::record()
{
    assert(videoFFmpeg.isRunning());
    assert(videoPipe.isOpen());

    recordVideo();
}

void
Recorder::recordVideo()
{
    /*
    auto *buffer = denise.pixelEngine.stablePtr();

    isize width = sizeof(u32) * (cutout.x2 - cutout.x1);
    isize height = cutout.y2 - cutout.y1;
    isize offset = cutout.y1 * HPIXELS + cutout.x1;
    u8 *src = (u8 *)(buffer + offset);
    u8 *dst = (u8 *)videoData.ptr;

    for (isize y = 0; y < height; y++, src += sizeof(u32) * HPIXELS, dst += width) {
        std::memcpy(dst, src, width);
    }

    // Feed the video pipe
    assert(videoPipe.isOpen());
    isize length = width * height;
    isize written = videoPipe.write((u8 *)videoData.ptr, length);

    if (written != length || FORCE_RECORDING_ERROR) {
        state = State::abort;
    }
    */
}

void
Recorder::finalize()
{
    // Close pipe
    videoPipe.close();

    // Wait for the decoder to terminate
    videoFFmpeg.join();

    // Switch state
    state = State::wait;
    recStop = Time::now();
}

void
Recorder::abort()
{
    finalize();
}

}
