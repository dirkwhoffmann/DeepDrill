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
#include "Traveller.h"
#include "Options.h"
#include "Animated.h"
#include "Logger.h"

#include <SFML/Graphics.hpp>

namespace dd {

Traveller::Traveller(Options &o) : opt(o)
{
    init();
}

void
Traveller::init()
{
    auto sourceWidth = unsigned(opt.image.width);
    auto sourceHeight = unsigned(opt.image.height);
    auto targetWidth = unsigned(opt.video.width);
    auto targetHeight = unsigned(opt.video.height);

    // Create the render window
    auto videoMode = sf::VideoMode(targetWidth, targetHeight);
    window.create(videoMode, "Preview");

    // Preview in real-time when no video is recorded
    // TODO: if !video
    window.setFramerateLimit(60);

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
    auto shaderSource =
    "uniform sampler2D texture;"
    "void main() {"
    "vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);"
    // "gl_FragColor = gl_Color * pixel * vec4(0.0,1.0,1.0,1.0);"
    "gl_FragColor = gl_Color * pixel;"
    "}";

    if (!shader.loadFromMemory(shaderSource, sf::Shader::Fragment)) {
        throw std::runtime_error("Can't load fragment shader");
    }

}

void
Traveller::launch()
{
    isize frame = 0;
    isize image = 0;

    Animated2 w, h;

    auto width = unsigned(opt.image.width);
    auto height = unsigned(opt.image.height);
    auto inbetweens = opt.video.inbetweens;

    recorder.startRecording();

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        w.move();
        h.move();

        if (frame++ % (inbetweens) == 0) {

            try {

                updateTexture(image++);

                w.set(opt.image.width);
                h.set(opt.image.height);
                w.set(opt.image.width / 2.0, inbetweens);
                h.set(opt.image.height / 2.0, inbetweens);

            } catch (...) {

                break;
            }
        }

        if (opt.verbose) {
            // printf("%f %f %f %f\n", w.current, w.factor, h.current, h.factor);
        }
        auto newRect = sf::IntRect(unsigned((width - w.current) / 2.0),
                                   unsigned((height - h.current) / 2.0),
                                   unsigned(w.current),
                                   unsigned(h.current));
        sourceRect.setTextureRect(newRect);

        // Render target texture
        shader.setUniform("texture", source);
        target.draw(sourceRect, &shader);
        target.display();

        // Draw target texture in the preview window
        window.clear();
        window.draw(targetRect);
        window.display();

        if (!opt.output.empty()) {

            // Read back image data
            auto image = target.getTexture().copyToImage();

            // Record frame
            recorder.record(image);
        }
    }

    recorder.stopRecording();
}

void
Traveller::updateTexture(isize nr)
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

    printf("Switched to texture %ld\n", nr);
}

}
