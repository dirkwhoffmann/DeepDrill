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

#include <filesystem>
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

    auto sourceWidth = unsigned(opt.image.width);
    auto sourceHeight = unsigned(opt.image.height);
    auto targetWidth = unsigned(opt.video.width);
    auto targetHeight = unsigned(opt.video.height);

    // Create the render window
    auto videoMode = sf::VideoMode(targetWidth, targetHeight);
    window.create(videoMode, "");

    // Hide the window in batch mode
    if (opt.flags.batch) window.setVisible(false);

    // Preview in real-time if no video is recorded
    window.setFramerateLimit(recordMode ? 0 : unsigned(opt.video.frameRate));

    // Create the source texture
    if (!source.create(sourceWidth, sourceHeight)) {
        throw Exception("Can't create source texture");
    }
    sourceRect.setSize(sf::Vector2f(targetWidth, targetHeight));
    sourceRect.setTexture(&source);

    // Create the scale textures
    for (isize i = 0; i < 3; i++) {

        if (!scaled[i].create(targetWidth, targetHeight)) {
            throw Exception("Can't create scale texture " + std::to_string(i));
        }
        scaledRect[i].setSize(sf::Vector2f(targetWidth, targetHeight));
        scaledRect[i].setTexture(&scaled[i].getTexture());
    }

    // Create the target texture
    if (!target.create(targetWidth, targetHeight)) {
        throw Exception("Can't create target texture");
    }
    targetRect.setSize(sf::Vector2f(targetWidth, targetHeight));
    targetRect.setTexture(&target.getTexture());

    // Load shaders
    auto scalerPath = opt.assets.findAsset(opt.video.scaler, Format::GLSL);
    auto mergerPath = opt.assets.findAsset(opt.video.merger, Format::GLSL);

    if (scalerPath == "") {
        throw Exception("Can't load fragment shader '" + opt.video.scaler + "'");
    }
    if (mergerPath == "") {
        throw Exception("Can't load fragment shader '" + opt.video.merger + "'");
    }

    scaler.loadFromFile(scalerPath, sf::Shader::Fragment);
    merger.loadFromFile(mergerPath, sf::Shader::Fragment);
}

void
Zoomer::launch()
{
    sf::Event event;

    // Start FFmpeg
    if (recordMode) recorder.startRecording();

    // Process all keyframes
    for (isize keyframe = 0; keyframe < opt.video.keyframes; keyframe++) {

        ProgressIndicator progress("Processing keyframe " + std::to_string(keyframe), opt.video.inbetweens);

        // Process all inbetweens
        for (isize frame = 0; frame < opt.video.inbetweens; frame++) {

            // Process all events
            if (!window.isOpen()) throw UserInterruptException();
            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed)
                    window.close();
            }

            // Update state
            update(keyframe, frame);

            // Render frame
            draw();

            progress.step(1);
        }
    }

    // Stop FFmpeg
    if (recordMode) recorder.stopRecording();
}

void
Zoomer::update(isize keyframe, isize frame)
{
    if (frame == 0) {

        isize dx;
        isize dy;

        updateTexture(keyframe);
        updateLocation(keyframe, dx, dy);

        // Set animation start point
        x.set(Coord::center(opt).x);
        y.set(Coord::center(opt).y);
        w.set(opt.image.width);
        h.set(opt.image.height);

        // Set animation end point and speed
        x.set(Coord::center(opt).x + dx, opt.video.inbetweens);
        y.set(Coord::center(opt).y - dy, opt.video.inbetweens);
        w.set(opt.image.width / 2.0, opt.video.inbetweens);
        h.set(opt.image.height / 2.0, opt.video.inbetweens);

        // Update window title bar
        string title = "DeepFlight - ";
        title += recordMode ? "Recording " : "Preview ";
        title += "[Keyframe " + std::to_string(keyframe + 1);
        title += " / " + std::to_string(opt.video.keyframes) + "] ";
        window.setTitle(title);

    } else {

        x.move();
        y.move();
        w.move();
        h.move();
    }

    auto newRect = sf::IntRect(unsigned(x.current - (w.current / 2.0)),
                               unsigned(y.current - (h.current / 2.0)),
                               unsigned(w.current),
                               unsigned(h.current));
    sourceRect.setTextureRect(newRect);
}

void
Zoomer::draw()
{
    // Stage 1: Scale down the source texture
    scaler.setUniform("texture", source);
    scaled[latest].draw(sourceRect, &scaler);
    scaled[latest].display();

    // Stage 2: Render target texture
    merger.setUniform("current", scaled[latest].getTexture());
    merger.setUniform("prev", scaled[(latest + 3 - 1) % 3].getTexture());
    merger.setUniform("prevprev", scaled[(latest + 3 - 2) % 3].getTexture());
    target.draw(scaledRect[latest], &merger);
    target.display();
    latest = (latest + 1) % 3;

    // Draw target texture in the preview window
    window.clear();
    window.draw(targetRect);
    window.display();

    if (recordMode) {

        // Read back image data
        auto image = target.getTexture().copyToImage();

        // Record frame
        recorder.record(image);
    }
}

void
Zoomer::updateTexture(isize nr)
{
    auto path = opt.files.input.parent_path() / opt.files.input.stem();
    string name = path.string() + "_" + std::to_string(nr + 1) + ".png";

    sf::Texture image;

    if (!fileExists(name)) {
        throw FileNotFoundError(name);
    }
    if (!source.loadFromFile(name)) {
        throw Exception("Can't load image file " + name);
    }
}

void
Zoomer::updateLocation(isize nr, isize &dx, isize &dy)
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

        auto centerDelta = (opt.center - oldCenter) / oldPixelDelta;
        dx = isize(std::round(centerDelta.re.get_d()));
        dy = isize(std::round(centerDelta.im.get_d()));

        // printf("dx: %ld dy: %ld\n", dx, dy);

    } else {

        dx = 0;
        dy = 0;
    }
}

}
