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
    recordMode = opt.files.output != "";

    auto mapRes = sf::Vector2u(unsigned(opt.drillmap.width), unsigned(opt.drillmap.height));
    auto vidRes = sf::Vector2u(unsigned(opt.video.width), unsigned(opt.video.height));

    // Create the render window
    window.create(sf::VideoMode(vidRes.x, vidRes.y), "");

    // Hide the window in batch mode
    if (opt.flags.batch) window.setVisible(false);

    // Preview in real-time if no video is recorded
    window.setFramerateLimit(recordMode ? 0 : unsigned(opt.video.frameRate));

    // Create textures
    initTexture(source1, mapRes);
    initTexture(normal1, mapRes);
    initTexture(source2, mapRes);
    initTexture(normal2, mapRes);

    // Setup GPU filters
    illuminator1.init(opt.image.illuminator, mapRes);
    illuminator2.init(opt.image.illuminator, mapRes);
    downscaler.init(opt.video.scaler, vidRes);
}

void
Zoomer::initTexture(sf::Texture &tex, sf::Vector2u size)
{
    if (!tex.create(size.x, size.y)) {
        throw Exception("Can't create texture");
    }
    tex.setSmooth(false);
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

        // Process all inbetweens
        for (frame = 0; frame < opt.video.inbetweens; frame++) {

            // Process all events
            if (!window.isOpen()) throw UserInterruptException();
            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed)
                    window.close();
            }

            // Update state
            update();

            // Render frame
            draw();

            progress.step(1);
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

    // 1. Colorize
    drillMap.colorize();
    drillMap2.colorize();

    // 2. Scale down
    downscaler.apply([this](sf::Shader &shader) {

        shader.setUniform("curr", drillMap.getTexture());
        shader.setUniform("next", drillMap2.getTexture());
        shader.setUniform("size", sf::Vector2f(opt.drillmap.width, opt.drillmap.height));
        shader.setUniform("zoom", float(zoom.current));
        shader.setUniform("frame", (float)frame / (float)opt.video.inbetweens);
    });

    // 3. Display the result
    window.clear();
    window.draw(downscaler.getRect());
    window.display();


    if (recordMode) {

        // Read back image data
        auto image = downscaler.getTexture().copyToImage();

        // Record frame
        recorder.record(image);
    }
}

void
Zoomer::updateTextures(isize nr)
{

    auto mapFile = [&](isize nr) {

        auto path = opt.files.input.parent_path() / opt.files.input.stem();
        return path.string() + "_" + std::to_string(nr) + ".map";
    };

    {   SILENT

        // Load map files
        if (!fileExists(mapFile(nr))) {
            throw FileNotFoundError(mapFile(nr));
        }
        drillMap.load(mapFile(nr));
        drillMap.colorMap.update(drillMap);

        if (!fileExists(mapFile(nr + 1))) {
            throw FileNotFoundError(mapFile(nr + 1));
        }
        drillMap2.load(mapFile(nr + 1));
        drillMap2.colorMap.update(drillMap2);

        // Create textures

        /*
        auto &colorMap1 = drillMap.colorize();
        source1.update((u8 *)colorMap1.colorMap.ptr);
        normal1.update((u8 *)colorMap1.normalMap.ptr);

        auto &colorMap2 = drillMap2.colorize();
        source2.update((u8 *)colorMap2.colorMap.ptr);
        normal2.update((u8 *)colorMap2.normalMap.ptr);
        */

    }
}

void
Zoomer::updateLocation(isize nr)
{
    auto path = opt.files.input.parent_path() / opt.files.input.stem();
    string name = path.string() + "_" + std::to_string(nr + 1) + ".loc";

    // Read the first location file if this is the first location update
    if (nr == 0) {

        string name = path.string() + "_0.loc";
        Parser::parse(name, [this](string k, string v) { opt.parse(k,v); });
        opt.derive();
    }

    // Read the next location file if existent
    if (fileExists(name)) {

        auto oldCenter = opt.center;
        auto oldPixelDelta = opt.mpfPixelDelta;

        Parser::parse(name, [this](string k, string v) { opt.parse(k,v); });
        opt.derive();
    }
}

}
