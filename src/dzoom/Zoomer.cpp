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

namespace fs = std::filesystem;

namespace dd {

Zoomer::Zoomer(Options &o) : opt(o)
{
    init();
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

void
Zoomer::launch()
{
    sf::Event event;

    // Start FFmpeg
    if (recordMode) recorder.startRecording();

    // Process all keyframes
    for (keyframe = 0; keyframe < opt.video.keyframes; keyframe++) {

        ProgressIndicator progress("Processing keyframe " + std::to_string(keyframe), opt.video.inbetweens);

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

            // Update state
            updateClock.go();
            update();
            updateClock.stop();

            // Render frame
            renderClock.go();
            draw();
            renderClock.stop();

            // Record frame
            recordClock.go();
            record();
            recordClock.stop();

            progress.step(1);
        }

        progress.done();

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
    }

    // Stop FFmpeg
    if (recordMode) recorder.stopRecording();
}

void
Zoomer::update()
{
    if (frame == 0) {

        updateTextures(keyframe);
        updateLocation(keyframe);

        // Set animation start point
        zoom.set(1.0);

        // Set animation end point and speed
        zoom.set(2.0, opt.video.inbetweens);

        // Update window title bar
        string title = "DeepZoom - ";
        title += recordMode ? "Recording " : "Preview ";
        title += "[Keyframe " + std::to_string(keyframe + 1);
        title += " / " + std::to_string(opt.video.keyframes) + "] ";
        window.setTitle(title);

    } else {

        zoom.move();
    }
}

void
Zoomer::draw()
{
    // Colorize
    colorizer.draw(drillMap.colorMap, drillMap2.colorMap,
                   (float)frame / (float)opt.video.inbetweens,
                   float(zoom.current));

    // Display the result
    window.clear();
    window.draw(colorizer.getRect());
    window.display();
}

void
Zoomer::record()
{
    if (recordMode) {

        recorder.record(colorizer.getImage());
    }
}

void
Zoomer::updateTextures(isize nr)
{

    auto mapFile = [&](isize nr) {

        fs::path input = opt.files.inputs.front();
        fs::path path = input.parent_path() / input.stem();
        return path.string() + "_" + std::to_string(nr) + ".map";
    };

    {   SILENT

        // Load map file of the current keyframe
        if (!fileExists(mapFile(nr))) {
            throw FileNotFoundError(mapFile(nr));
        }
        drillMap.load(mapFile(nr));
        drillMap.colorize();

        // Load mapfile of the next frame
        if (!fileExists(mapFile(nr + 1))) {
            throw FileNotFoundError(mapFile(nr + 1));
        }
        drillMap2.load(mapFile(nr + 1));
        drillMap2.colorize();
    }
}

void
Zoomer::updateLocation(isize nr)
{
    /*
    fs::path input = opt.files.inputs.front();
    fs::path path = input.parent_path() / input.stem();
    string name = path.string() + "_" + std::to_string(nr + 1) + ".loc";

    // Read the first location file if this is the first location update
    if (nr == 0) {

        string name = path.string() + "_0.loc";
        Parser::parse(name, [this](string k, string v) { opt.parse(k,v); });
        opt.derive();
    }

    // Read the next location file if existent
    if (fileExists(name)) {

        Parser::parse(name, [this](string k, string v) { opt.parse(k,v); });
        opt.derive();
    }
    */
}

}
