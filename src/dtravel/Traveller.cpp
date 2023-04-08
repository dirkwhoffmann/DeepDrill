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
    auto width = unsigned(opt.image.width);
    auto height = unsigned(opt.image.height);
    // auto frames = opt.video.frames;

    // Create the render window
    auto videoMode = sf::VideoMode(width, height);
    window.create(videoMode, "Preview");
    window.setFramerateLimit(60);

    // Create textures
    if (!source.create(width, height)) {
        throw Exception("Can't create source texture");
    }
    if (!target.create(width, height)) {
        throw Exception("Can't create target texture");
    }
    sourceRect.setSize(sf::Vector2f(width, height));
    sourceRect.setTexture(&source);
    targetRect.setSize(sf::Vector2f(width, height));
    targetRect.setTexture(&target.getTexture());

    // Load shader
    auto shaderSource =
    "uniform sampler2D texture;"
    "void main() {"
    "vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);"
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

    Animated<float> x1, y1, x2, y2;

    auto width = unsigned(opt.image.width);
    auto height = unsigned(opt.image.height);
    auto frames = opt.video.frames;

    recorder.startRecording(4096, 4, 3);

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (frame++ % frames == 0) {

            try {

                updateTexture(image++);

                x1.set(0);
                y1.set(0);
                x2.set(width);
                y2.set(height);

                x1.set(width / 4, frames);
                y1.set(height / 4, frames);
                x2.set(width * 3 / 4, frames);
                y2.set(height * 3 / 4, frames);

            } catch (...) {

                break;
            }
        }

        x1.move();
        y1.move();
        x2.move();
        y2.move();

        int x1i = int(x1.current);
        int y1i = int(y1.current);
        int x2i = int(x2.current);
        int y2i = int(y2.current);

        auto newRect = sf::IntRect(x1i, y2i, x2i - x1i, -(y2i - y1i));
        sourceRect.setTextureRect(newRect);

        window.clear();
        // window.draw(sourceRect);

        // Render target texture
        shader.setUniform("texture", source);
        target.draw(sourceRect, &shader);

        // Draw target texture
        window.draw(targetRect);
        window.display();
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
