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

#include "config.h"
#include "Zoomer.h"
#include "Coord.h"
#include "Options.h"
#include "Logger.h"

#include <SFML/Graphics.hpp>

namespace dd {

Zoomer::Zoomer(Options &o) : opt(o)
{
    init();
}

void
Zoomer::init()
{
    auto sourceWidth = unsigned(opt.image.width);
    auto sourceHeight = unsigned(opt.image.height);
    auto targetWidth = unsigned(opt.video.width);
    auto targetHeight = unsigned(opt.video.height);

    // Create the render window
    auto videoMode = sf::VideoMode(targetWidth, targetHeight);
    window.create(videoMode, "");

    // Preview in real-time if no video is recorded
    window.setFramerateLimit(recordMode() ? 0 : unsigned(opt.video.frameRate));

    // Create textures
    if (!source.create(sourceWidth, sourceHeight)) {
        throw Exception("Can't create source texture");
    }
    if (!target.create(targetWidth, targetHeight)) {
        throw Exception("Can't create target texture");
    }
    sourceRect.setSize(sf::Vector2f(targetWidth, targetHeight));
    sourceRect.setTexture(&source);
    targetRect.setSize(sf::Vector2f(targetWidth, targetHeight));
    targetRect.setTexture(&target.getTexture());

    // Load shader
    /*
     auto shaderSource =
     "uniform sampler2D texture;"
     "void main() {"
     "vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);"
     // "gl_FragColor = gl_Color * pixel * vec4(0.0,1.0,1.0,1.0);"
     "gl_FragColor = gl_Color * pixel;"
     "}";
     */

    if (!shader.loadFromFile(opt.video.scaler, sf::Shader::Fragment)) {
        throw std::runtime_error("Can't load fragment shader '" + opt.video.scaler + "'");
    }
    /*
     if (!shader.loadFromMemory(shaderSource, sf::Shader::Fragment)) {
     throw std::runtime_error("Can't load fragment shader");
     }
     */
}

void
Zoomer::launch()
{
    isize keyframe = 0;
    isize frame = 0;

    // Start FFmpeg
    if (recordMode()) recorder.startRecording();

    // Enter main loop
    while (window.isOpen()) {

        sf::Event event;

        // Process events
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update state
        if (!update(keyframe, frame) && recordMode()) break;

        // Render frame
        draw();
    }

    // Stop FFmpeg
    if (recordMode()) recorder.stopRecording();
}

bool
Zoomer::update(isize &keyframe, isize &frame)
{
    if (keyframe >= opt.video.keyframes) {
        return false;
    }

    x.move();
    y.move();
    w.move();
    h.move();

    if (frame++ % opt.video.inbetweens == 0) {

        isize dx;
        isize dy;

        updateTexture(keyframe);
        updateLocation(keyframe, dx, dy);
        keyframe++;

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
        title += recordMode() ? "Recording " : "Preview ";
        title += "[Keyframe " + std::to_string(keyframe);
        title += " / " + std::to_string(opt.video.keyframes) + "] ";
        window.setTitle(title);
    }

    if (opt.verbose) {
        // printf("[%ld] %f %f %f %f\n", frame, x.current, y.current, w.current, h.current);
    }
    auto newRect = sf::IntRect(unsigned(x.current - (w.current / 2.0)),
                               unsigned(y.current - (h.current / 2.0)),
                               unsigned(w.current),
                               unsigned(h.current));
    sourceRect.setTextureRect(newRect);

    return true;
}

void
Zoomer::draw()
{
    // Render target texture
    shader.setUniform("texture", source);
    target.draw(sourceRect, &shader);
    target.display();

    // Draw target texture in the preview window
    window.clear();
    window.draw(targetRect);
    window.display();

    if (recordMode()) {

        // Read back image data
        auto image = target.getTexture().copyToImage();

        // Record frame
        recorder.record(image);
    }
}

bool
Zoomer::recordMode()
{
    return !opt.output.empty();
}

void
Zoomer::updateTexture(isize nr)
{
    string path = opt.input;
    string name = path + "_" + std::to_string(nr) + ".png";

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
    string path = opt.input;
    string name = path + "_" + std::to_string(nr + 1) + ".loc";

    // Read the first location file if this is the first location update
    if (nr == 0) {

        string name = path + "_0.loc";
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
