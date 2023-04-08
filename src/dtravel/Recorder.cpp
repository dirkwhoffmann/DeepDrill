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

Recorder::Recorder(const Options &opt) : opt(opt)
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

void
Recorder::startRecording(isize bitRate, isize aspectX, isize aspectY)
{
    assert(!videoPipe.isOpen());

    // Create pipe
    if (!videoPipe.create(videoPipePath())) {
        throw Exception("Failed to create the video encoder pipe.");
    }

    // Set the bit rate, frame rate, and sample rate
    this->bitRate = bitRate;
    frameRate = 60;

    // Create temporary buffers
    videoData.alloc(opt.image.width * opt.image.height);

    //
    // Assemble the command line arguments for the video encoder
    //

    // Console interactions
    string cmd = " -nostdin";

    // Verbosity
    cmd += " -loglevel " + loglevel();

    // Input stream format
    cmd += " -f:v rawvideo -pixel_format rgba";

    // Frame rate
    cmd += " -r " + std::to_string(frameRate);

    // Frame size (width x height)
    cmd += " -s:v " + std::to_string(opt.image.width) + "x" + std::to_string(opt.image.height);

    // Input source (named pipe)
    cmd += " -i " + videoPipePath();

    // Output stream format
    cmd += " -f mp4 -pix_fmt yuv420p";

    // Bit rate
    cmd += " -b:v " + std::to_string(bitRate) + "k";

    // Aspect ratio
    /*
    cmd += " -bsf:v ";
    cmd += "\"h264_metadata=sample_aspect_ratio=";
    cmd += std::to_string(aspectX) + "/" + std::to_string(2*aspectY) + "\"";
    */

    // Output file
    cmd += " -y " + videoStreamPath();

    //
    // Launch FFmpeg instance
    //

    assert(!videoFFmpeg.isRunning());

    // Launch the video encoder
    if (!videoFFmpeg.launch(cmd)) {
        throw Exception("Unable to launch the FFmpeg encoder.");
    }

    // Open the video pipe
    if (!videoPipe.open()) {
        throw Exception("Unable to open the video pipe.");
    }

    if (opt.verbose) {

        log::cout << log::vspace;
        log::cout << log::ralign("Number of images: ");
        log::cout << opt.video.images << log::endl;
        log::cout << log::ralign("Frames per image: ");
        log::cout << opt.video.frames << log::endl;
        log::cout << log::ralign("Duration: ");
        log::cout << opt.video.duration << " sec" << log::endl;
        log::cout << log::ralign("Bitrate: ");
        log::cout << opt.video.bitrate << log::endl;
        log::cout << log::vspace;
    }
}

void
Recorder::stopRecording()
{
    assert(videoPipe.isOpen());

    // Close pipe
    videoPipe.close();

    // Wait for the decoder to terminate
    videoFFmpeg.join();
}

void
Recorder::record()
{
    assert(videoFFmpeg.isRunning());
    assert(videoPipe.isOpen());

    // TODO
}

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
