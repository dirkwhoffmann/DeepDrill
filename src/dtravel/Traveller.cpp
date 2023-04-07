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

}

void
Traveller::launch()
{
    Animated<float> x1, y1, x2, y2;

    auto width = unsigned(opt.image.width);
    auto height = unsigned(opt.image.height);
    auto frames = opt.video.frames;

    // Create render window
    auto videoMode = sf::VideoMode(width, height);
    window.create(videoMode, "Preview");
    window.setFramerateLimit(60);

    // Create the texture
    if (!texture.create(width, height)) {
        throw Exception("Can't create texture");
    }
    textureRect.setSize(sf::Vector2f(width, height));
    textureRect.setTexture(&texture.getTexture());

    auto rect = textureRect.getTextureRect();
    auto newRect = sf::IntRect(0, rect.height, rect.width, -rect.height);
    textureRect.setTextureRect(newRect);

    isize frame = 0;
    isize image = 0;

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

            } catch (...) { }
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
        textureRect.setTextureRect(newRect);

        window.clear();
        window.draw(textureRect);
        window.display();
    }
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
    if (!image.loadFromFile(name)) {
        throw Exception("Can't load image file " + name);
    }
    sf::Sprite sprite(image);
    texture.draw(sprite);

    printf("Switched to texture %ld\n", nr);
}

}
