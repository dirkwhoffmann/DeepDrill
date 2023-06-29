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

Zoomer::Zoomer(Options &o) : opt(o)
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
    recordMode = !opt.files.outputs.empty();

    // Create the render window
    auto mode = sf::VideoMode(unsigned(opt.image.width), unsigned(opt.image.height));
    window.create(mode, "");

    // Hide the window in batch mode
    if (opt.flags.batch) window.setVisible(false);

    // Preview in real-time if no video is recorded
    window.setFramerateLimit(recordMode ? 0 : unsigned(opt.video.frameRate));

    // Initialize the colorizer
    colorizer.init(opt.image.illuminator, opt.video.scaler);
}

/*
void
Zoomer::launch()
{
    sf::Event event;

    // Start FFmpeg
    if (recordMode) recorder.startRecording();

    // Load the textures of the first two keyframes
    log::cout << log::vspace;
    log::cout << "Preloading map file 0" << log::endl << log::endl;
    (void)loadMapFile(0);

    log::cout << log::vspace;
    log::cout << "Preloading map file 1" << log::endl << log::endl;
    (void)loadMapFile(1);

    // Process all keyframes
    for (keyframe = 0; keyframe < opt.video.keyframes; keyframe++) {

        log::cout << log::vspace;
        log::cout << "Zooming from keyframe " << std::to_string(keyframe);
        log::cout << " to keyframe " << std::to_string(keyframe + 1) << ": ";
        log::cout << std::to_string(opt.video.inbetweens) << " inbetweens";
        log::cout << log::endl << log::endl;

        updateClock.reset();
        renderClock.reset();
        recordClock.reset();

        // Process all inbetweens
        for (frame = 0; frame < opt.video.inbetweens; frame++) {

            // Process all events
            if (!window.isOpen()) throw UserInterruptException();
            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed)
                    window.close();
            }

            //Perform main tasks
            update();
            draw();
            record();

            // progress.step(1);
        }

        // progress.done();

        if (opt.flags.verbose) {

            log::cout << log::vspace;
            log::cout << log::ralign("Update: ");
            log::cout << updateClock.getElapsedTime() << log::endl;
            log::cout << log::ralign("Render: ");
            log::cout << renderClock.getElapsedTime() << log::endl;

            if (recordMode) {

                log::cout << log::ralign("Record: ");
                log::cout << recordClock.getElapsedTime() << log::endl;
            }
            log::cout << log::vspace;
        }

        // Wait for the async map file loader to finish
        (void)loadResult.get();
    }

    // Stop FFmpeg
    if (recordMode) recorder.stopRecording();
}
*/

void
Zoomer::launch()
{
    sf::Event event;

    // Start FFmpeg
    if (recordMode) recorder.startRecording();

    // Set animation parameter
    zoom.set(1.0);
    zoom.set(4.0, 2 * opt.video.inbetweens);

    // Experimental (REMOVE ASAP)
    /*
    zoom.set(2.0);
    zoom.set(0.5, 2 * opt.video.inbetweens);
    keyframe = 12;
    */

    isize oldKeyframe = -1;

    // Process all frames
    for (frame = 0;; frame++) {

        // Exit if the user has closed the window
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
Zoomer::animate()
{
    zoom.move();

    bool switched = false;

    if (zoom.current >= 2.0) {

        // Switch to the next keyframe
        keyframe++;
        switched = true;
        zoom.current /= 2.0;
    }

    if (zoom.current < 1.0) {

        // Switch to the previous keyframe
        keyframe--;
        switched = true;
        zoom.current *= 2.0;
    }
}

void
Zoomer::update()
{
    updateClock.go();

    // Terminate if we've left the valid keyframe range
    if (keyframe < 0 || keyframe > opt.video.keyframes) throw Exit();

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
    colorizer.draw(drillMap[slotNr(keyframe + 0)].colorMap,
                   drillMap[slotNr(keyframe + 1)].colorMap,
                   (float)frame / (float)opt.video.inbetweens,
                   float(zoom.current));

    // Display the result
    window.clear();
    window.draw(colorizer.getRect());
    window.display();

    renderClock.stop();
}

void
Zoomer::record()
{
    if (recordMode) {

        recordClock.go();
        recorder.record(colorizer.getImage());
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
    fs::path path = opt.files.inputs.front();
    fs::path file = path / AssetManager::mapFile(nr);

    drillMap[slotNr(nr)].load(file);
    drillMap[slotNr(nr)].colorize();

    return true;
}

void
Zoomer::report()
{
    static isize oldKeyFrame = -1;

    if (oldKeyFrame != keyframe) {

        oldKeyFrame = keyframe;

        if (frame != 0) {

            progress.done();

            if (opt.flags.verbose) {

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
                      opt.video.inbetweens);

        // Update the title bar of the preview window
        string title = "DeepZoom - ";
        title += recordMode ? "Recording " : "Preview ";
        title += "[Keyframe " + std::to_string(keyframe);
        title += " / " + std::to_string(opt.video.keyframes) + "] ";
        window.setTitle(title);
    }

    progress.step();
}

}
