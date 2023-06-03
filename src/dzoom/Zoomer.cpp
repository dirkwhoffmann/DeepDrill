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

    auto videoW = unsigned(opt.video.width);
    auto videoH = unsigned(opt.video.height);
    auto imageDim = sf::Vector2u(unsigned(opt.image.width), unsigned(opt.image.height));
    auto videoDim = sf::Vector2u(unsigned(opt.video.width), unsigned(opt.video.height));

    // Create the render window
    auto videoMode = sf::VideoMode(videoW, videoH);
    window.create(videoMode, "");

    // Hide the window in batch mode
    if (opt.flags.batch) window.setVisible(false);

    // Preview in real-time if no video is recorded
    window.setFramerateLimit(recordMode ? 0 : unsigned(opt.video.frameRate));

    // Create textures
    initTexture(source, sourceRect, imageDim);
    initTexture(normal, imageDim);
    initTexture(source2, sourceRect2, imageDim);
    initTexture(normal2, imageDim);
    initRenderTexture(illuminated, illuminatedRect, imageDim);
    initRenderTexture(illuminated2, illuminatedRect2, imageDim);
    initRenderTexture(scaled, scaledRect, videoDim);

    // Load shaders
    initShader(scaler, opt.video.scaler);
    initShader(illuminator, opt.video.illuminator);
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
Zoomer::initTexture(sf::Texture &tex, sf::RectangleShape &rect, sf::Vector2u size)
{
    initTexture(tex, size);
    rect.setSize(sf::Vector2f(size.x, size.y));
    rect.setTexture(&tex);
}

void
Zoomer::initRenderTexture(sf::RenderTexture &tex, sf::Vector2u size)
{
    if (!tex.create(size.x, size.y)) {
        throw Exception("Can't create render texture");
    }
    tex.setSmooth(false);
}

void
Zoomer::initRenderTexture(sf::RenderTexture &tex, sf::RectangleShape &rect, sf::Vector2u size)
{
    initRenderTexture(tex, size);
    rect.setSize(sf::Vector2f(size.x, size.y));
    rect.setTexture(&tex.getTexture());
}

void
Zoomer::initShader(sf::Shader &shader, const string &name)
{
    auto path = opt.assets.findAsset(name, Format::GLSL);

    if (path == "") {
        throw Exception("Can't load fragment shader '" + name + "'");
    }
    if (!shader.loadFromFile(path, sf::Shader::Fragment)) {
        throw Exception("Can't load fragment shader '" + path.string() + "'");
    }
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
            draw(keyframe, frame);

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

        updateTextures(keyframe);
        updateLocation(keyframe);

        // Set animation start point
        w.set(opt.image.width);
        h.set(opt.image.height);

        // Set animation end point and speed
        w.set(opt.image.width / 2.0, opt.video.inbetweens);
        h.set(opt.image.height / 2.0, opt.video.inbetweens);

        // Update window title bar
        string title = "DeepZoom - ";
        title += recordMode ? "Recording " : "Preview ";
        title += "[Keyframe " + std::to_string(keyframe + 1);
        title += " / " + std::to_string(opt.video.keyframes) + "] ";
        window.setTitle(title);

    } else {

        w.move();
        h.move();
    }

    auto newRect = sf::IntRect(int(Coord::center(opt).x - (w.current / 2.0)),
                               int(Coord::center(opt).y + (h.current / 2.0)),
                               int(w.current),
                               int(-h.current));

    illuminatedRect.setTextureRect(newRect);
    illuminatedRect2.setTextureRect(newRect);
}

void
Zoomer::draw(isize keyframe, isize frame)
{
    // Phase 1: Apply lighting
    setupIlluminatorUniforms(keyframe, frame);

    illuminator.setUniform("image", source);
    illuminator.setUniform("normal", normal);
    illuminated.draw(illuminatedRect, &illuminator);
    illuminated.display();

    illuminator.setUniform("image", source2);
    illuminator.setUniform("normal", normal2);
    illuminated2.draw(illuminatedRect2, &illuminator);
    illuminated2.display();

    // Phase 2: Scale down
    setupScalerUniforms(keyframe, frame);
    scaled.draw(scaledRect, &scaler);
    scaled.display();

    // Phase 3: Display the result

    window.clear();
    window.draw(scaledRect);
    window.display();

    if (recordMode) {

        // Read back image data
        auto image = scaled.getTexture().copyToImage();

        // Record frame
        recorder.record(image);
    }
}

void
Zoomer::setupScalerUniforms(isize keyframe, isize frame)
{
    scaler.setUniform("curr", illuminated.getTexture());
    scaler.setUniform("next", illuminated2.getTexture());
    scaler.setUniform("size", sf::Vector2f(illuminated.getSize()));
    scaler.setUniform("frame", (float)frame / (float)opt.video.inbetweens);
}

void
Zoomer::setupIlluminatorUniforms(isize keyframe, isize frame)
{
    auto a = opt.colors.alpha * 3.14159 / 180.0;
    auto b = opt.colors.beta * 3.14159 / 180.0;

    auto z = std::sin(b);
    auto l = std::cos(b);
    auto y = std::sin(a) * l;
    auto x = std::cos(a) * l;

    illuminator.setUniform("lightDir", sf::Vector3f(x, y, z));
}

void
Zoomer::updateTextures(isize nr)
{
    auto path = opt.files.input.parent_path() / opt.files.input.stem();
    string name = path.string() + "_" + std::to_string(nr) + ".png";
    string name2 = path.string() + "_" + std::to_string(nr + 1) + ".png";
    string nrmName = path.string() + "_" + std::to_string(nr) + "_nrm.png";
    string nrmName2 = path.string() + "_" + std::to_string(nr + 1) + "_nrm.png";

    sf::Texture image;

    if (!fileExists(name)) {
        throw FileNotFoundError(name);
    }
    if (!source.loadFromFile(name)) {
        throw Exception("Can't load image file " + name);
    }
    if (!fileExists(name2)) {
        throw FileNotFoundError(name2);
    }
    if (!source2.loadFromFile(name2)) {
        throw Exception("Can't load image file " + name2);
    }
    if (fileExists(nrmName) && !normal.loadFromFile(nrmName)) {
        throw Exception("Can't load image file " + nrmName);
    }
    if (fileExists(nrmName2) && !normal2.loadFromFile(nrmName2)) {
        throw Exception("Can't load image file " + nrmName2);
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
