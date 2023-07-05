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

#include "Zoomer.h"
#include "Coord.h"
#include "DynamicFloat.h"
#include "Exception.h"
#include "IO.h"
#include "Options.h"
#include "Parser.h"
#include "ProgressIndicator.h"

#include <SFML/Graphics.hpp>
#include <thread>
#include <future>

namespace fs = std::filesystem;

namespace dd {

Zoomer::Zoomer()
{
    init();
}

Zoomer::~Zoomer()
{
    recorder.stopRecording();
}

void
Zoomer::init()
{
    recordMode = !Options::files.outputs.empty();

    // Create the render window
    auto mode = sf::VideoMode(unsigned(Options::image.width), unsigned(Options::image.height));
    window.create(mode, "");

    // Hide the window in batch mode
    if (Options::flags.batch) window.setVisible(false);

    // Preview in real-time if no video is recorded
    window.setFramerateLimit(recordMode ? 0 : unsigned(Options::video.frameRate));

    // Initialize the imageMaker
    imageMaker.init(Options::gpu.colorizer, Options::gpu.illuminator, Options::gpu.scaler);
}

void
Zoomer::launch()
{
    sf::Event event;

    // Initialize parameters
    keyframe = Options::video.startframe;
    zoom.set(1.0);

    // Start FFmpeg
    if (recordMode) recorder.startRecording();

    // Reset clocks
    updateClock.reset();
    renderClock.reset();
    recordClock.reset();

    // Enter the main loop
    for (frame = 0;; frame++) {

        // Exit if the preview window has been closed
        if (!window.isOpen()) throw UserInterruptException();

        // Process all events
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Perform main tasks
        report();
        animate();
        update();
        draw();
        record();
    }
}

void
Zoomer::report()
{
    static isize oldKeyFrame = -1;

    if (oldKeyFrame != keyframe) {

        oldKeyFrame = keyframe;

        if (frame != 0) {

            progress.done();

            if (Options::flags.verbose) {

                log::cout << log::vspace;
                log::cout << log::ralign("Update: ");
                log::cout << updateClock.reset() << log::endl;
                log::cout << log::ralign("Render: ");
                log::cout << renderClock.reset() << log::endl;

                if (recordMode) {

                    log::cout << log::ralign("Record: ");
                    log::cout << recordClock.reset() << log::endl;
                }
                log::cout << log::vspace;
            }
        }

        progress.init("Processing keyframe " + std::to_string(keyframe),
                      Animated::scale / Options::video.velocity(double(frame) / double(Options::video.frameRate)));

        // Update the title bar of the preview window
        string title = "DeepZoom - ";
        title += recordMode ? "Recording " : "Preview ";
        title += "[Keyframe " + std::to_string(keyframe);
        title += " / " + std::to_string(Options::video.keyframes) + "] ";
        window.setTitle(title);
    }

    progress.step();
}

void
Zoomer::animate()
{
    // Zoom in or out
    zoom.move(Options::video.velocity(double(frame) / double(Options::video.frameRate)));

    // Check if we need to switch to the next keyframe
    if (zoom.current >= 2.0) {

        keyframe++;
        zoom.current /= 2.0;
    }

    // Check if we need to switch to the previous keyframe
    if (zoom.current < 1.0) {

        keyframe--;
        zoom.current *= 2.0;
    }
}

void
Zoomer::update()
{
    updateClock.go();

    // Terminate if we've left the valid keyframe range
    if (keyframe < 0 || keyframe > Options::video.keyframes) throw Exit();

    // Check all map slots for dirty drill maps
    for (isize i = -1; i <= 2; i++) {

        auto frameNr = keyframe + i;
        auto nr = slotNr(frameNr);

        if (slot[nr] != frameNr) {

            // Update the map slot
            slot[nr] = frameNr;
            mapState[nr] = MapState::Loading;
            loadResult[nr] = loadMapFileAsync(frameNr);
        }
    }

    // Wait for the drill maps we need
    for (isize i = keyframe; i <= keyframe + 1; i++) {

        if (mapState[slotNr(i)] != MapState::UpToDate) {

            (void)loadResult[slotNr(i)].get();
            mapState[slotNr(i)] = MapState::UpToDate;
        }
    }

    updateClock.stop();
}

void
Zoomer::draw()
{
    renderClock.go();

    // Colorize
    imageMaker.draw(drillMap[slotNr(keyframe + 0)],
                    drillMap[slotNr(keyframe + 1)],
                    frame,
                    float(zoom.current));

    // Display the result
    window.clear();
    window.draw(imageMaker.getRect());
    window.display();

    renderClock.stop();
}

void
Zoomer::record()
{
    if (recordMode) {

        recordClock.go();
        recorder.record(imageMaker.getImage());
        recordClock.stop();
    }
}

std::future<bool>
Zoomer::loadMapFileAsync(isize nr)
{
    // Preload the next texture in the background
    return std::async([this, nr]() {

        auto result = loadMapFile(nr);
        return result;
    });
}

bool
Zoomer::loadMapFile(isize nr)
{
    fs::path path = Options::files.inputs.front();
    fs::path file = path / AssetManager::mapFile(nr);

    drillMap[slotNr(nr)].load(file);

    return true;
}

}
