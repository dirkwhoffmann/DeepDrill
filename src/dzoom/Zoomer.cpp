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
    illuminator1.init(opt.video.illuminator, mapRes);
    illuminator2.init(opt.video.illuminator, mapRes);
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
    // Phase 1: Apply lighting
    illuminator1.apply([this](sf::Shader &shader) {

        // Factor angle stuff out
        auto a = opt.colors.alpha * 3.14159 / 180.0;
        auto b = opt.colors.beta * 3.14159 / 180.0;

        auto z = std::sin(b);
        auto l = std::cos(b);
        auto y = std::sin(a) * l;
        auto x = std::cos(a) * l;

        shader.setUniform("lightDir", sf::Vector3f(x, y, z));
        shader.setUniform("image", source1);
        shader.setUniform("normal", normal1);
    });

    illuminator2.apply([this](sf::Shader &shader) {

        // Factor angle stuff out
        auto a = opt.colors.alpha * 3.14159 / 180.0;
        auto b = opt.colors.beta * 3.14159 / 180.0;

        auto z = std::sin(b);
        auto l = std::cos(b);
        auto y = std::sin(a) * l;
        auto x = std::cos(a) * l;

        shader.setUniform("lightDir", sf::Vector3f(x, y, z));
        shader.setUniform("image", source2);
        shader.setUniform("normal", normal2);
    });

    // Phase 2: Scale down
    downscaler.apply([this](sf::Shader &shader) {

        shader.setUniform("curr", illuminator1.getTexture());
        shader.setUniform("next", illuminator2.getTexture());
        shader.setUniform("size", sf::Vector2f(illuminator1.getSize()));
        shader.setUniform("zoom", float(zoom.current));
        shader.setUniform("frame", (float)frame / (float)opt.video.inbetweens);
    });

    // Phase 3: Display the result

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

        if (!fileExists(mapFile(nr + 1))) {
            throw FileNotFoundError(mapFile(nr + 1));
        }
        drillMap2.load(mapFile(nr + 1));

        // Create textures
        colorMap.init(drillMap);
        source1.update((u8 *)colorMap.colorMap.ptr);
        normal1.update((u8 *)colorMap.normalMap.ptr);

        colorMap.init(drillMap2);
        source2.update((u8 *)colorMap.colorMap.ptr);
        normal2.update((u8 *)colorMap.normalMap.ptr);
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
