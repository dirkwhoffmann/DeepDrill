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
    isize frame = 0;
    isize image = 0;

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
        try { update(frame, image); } catch (FileNotFoundError &) { break; }

        // Render frame
        draw();
    }

    // Stop FFmpeg
    if (recordMode()) recorder.stopRecording();
}

void
Zoomer::update(isize &frame, isize &image)
{
    auto flip = [](sf::IntRect r) {
        return sf::IntRect(r.left, r.top + r.height, r.width, -r.height);
    };

    x.move();
    y.move();
    w.move();
    h.move();

    if (frame++ % opt.video.inbetweens == 0) {

        updateTexture(image);
        updateLocation(image);
        image++;

        x.set(Coord::center(opt).x);
        y.set(Coord::center(opt).y);
        w.set(opt.image.width);
        h.set(opt.image.height);

        x.set(Coord::center(opt).x + opt.animation.dx, opt.video.inbetweens);
        y.set(Coord::center(opt).y + opt.animation.dy, opt.video.inbetweens);
        w.set(opt.image.width / 2.0, opt.video.inbetweens);
        h.set(opt.image.height / 2.0, opt.video.inbetweens);

        auto progress = isize(100.0 * image / opt.video.keyframes);
        string title = "DeepFlight - ";
        title += recordMode() ? "Recording " : "Preview ";
        window.setTitle(title + "[" + std::to_string(progress) + "%]");
    }

    if (opt.verbose) {
        // printf("%f %f %f %f\n", x.current, y.current, w.current, h.current);
    }
    auto newRect = sf::IntRect(unsigned(x.current - (w.current / 2.0)),
                               unsigned(y.current - (h.current / 2.0)),
                               unsigned(w.current),
                               unsigned(h.current));
    /*
    auto newRect = sf::IntRect(unsigned((opt.image.width - w.current) / 2.0),
                               unsigned((opt.image.height - h.current) / 2.0),
                               unsigned(w.current),
                               unsigned(h.current));
    */
    sourceRect.setTextureRect(flip(newRect));
    // sourceRect.setTextureRect((newRect));
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
        throw FileNotFoundError("File " + name + " does not exist");
    }
    if (!source.loadFromFile(name)) {
        throw Exception("Can't load image file " + name);
    }
    if (opt.verbose) {
        printf("Switched to texture %ld\n", nr);
    }
}

void
Zoomer::updateLocation(isize nr)
{
    string path = opt.input;
    string name = path + "_" + std::to_string(nr) + ".loc";

    if (opt.verbose) {
        printf("updateLocation: %s\n", name.c_str());
    }

    if (!fileExists(name)) {
        throw FileNotFoundError("File " + name + " does not exist");
    }

    map<string,string> keys;
    Parser::parse(name, keys);
    opt.parse(keys);

    printf("dx = %ld dy = %ld\n", opt.animation.dx, opt.animation.dy);
}

}
