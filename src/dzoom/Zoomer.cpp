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
    // initRenderTexture(illuminated, illuminatedRect, imageDim);
    // initRenderTexture(illuminated2, illuminatedRect2, imageDim);
    // initRenderTexture(scaled, scaledRect, videoDim);

    // Load shaders
    // initShader(scaler, opt.video.scaler);
    // initShader(illuminator, opt.video.illuminator);

    // Setup GPU filters
    illuminationFilter1.init(opt.video.illuminator, int(opt.image.width), int(opt.image.height));
    illuminationFilter2.init(opt.video.illuminator, int(opt.image.width), int(opt.image.height));
    scalingFilter.init(opt.video.scaler, videoW, videoH);
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
        /*
        w.set(opt.image.width);
        h.set(opt.image.height);
        */
        w.set(opt.video.width);
        h.set(opt.video.height);

        // Set animation end point and speed
        /*
        w.set(opt.image.width / 2.0, opt.video.inbetweens);
        h.set(opt.image.height / 2.0, opt.video.inbetweens);
        */
        w.set(opt.video.width / 2.0, opt.video.inbetweens);
        h.set(opt.video.height / 2.0, opt.video.inbetweens);

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

    /*
    auto newRect = sf::IntRect(int(Coord::center(opt).x / 2 - (w.current / 2.0)),
                               int(Coord::center(opt).y / 2 + (h.current / 2.0)),
                               int(w.current),
                               int(-h.current));
    */
    /*
    auto newRect2 = sf::IntRect(int(Coord::center(opt).x - (w.current / 2.0)) / 2,
                               int(Coord::center(opt).y + (h.current / 2.0)) / 2,
                               int(w.current) / 2,
                               int(-h.current) / 2);
     */

    // illuminatedRect.setTextureRect(newRect);
    // illuminatedRect2.setTextureRect(newRect);
    // illuminationFilter1.setTextureRect(newRect);
    // illuminationFilter2.setTextureRect(newRect);
    // scalingFilter.setTextureRect(newRect);
}

void
Zoomer::draw()
{
    // Phase 1: Apply lighting
    illuminationFilter1.apply([this](sf::Shader &shader) {

        // Factor angle stuff out
        auto a = opt.colors.alpha * 3.14159 / 180.0;
        auto b = opt.colors.beta * 3.14159 / 180.0;

        auto z = std::sin(b);
        auto l = std::cos(b);
        auto y = std::sin(a) * l;
        auto x = std::cos(a) * l;

        shader.setUniform("lightDir", sf::Vector3f(x, y, z));
        shader.setUniform("image", source);
        shader.setUniform("normal", normal);
    });

    illuminationFilter2.apply([this](sf::Shader &shader) {

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
    scalingFilter.apply([this](sf::Shader &shader) {

        shader.setUniform("curr", illuminationFilter1.getTexture());
        shader.setUniform("next", illuminationFilter2.getTexture());
        shader.setUniform("size", sf::Vector2f(illuminationFilter1.getSize()));
        shader.setUniform("frame", (float)frame / (float)opt.video.inbetweens);
    });

    // Phase 3: Display the result

    window.clear();
    window.draw(scalingFilter.getRect());
    window.display();

    if (recordMode) {

        // Read back image data
        auto image = scalingFilter.getTexture().copyToImage();

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
        colorizer.compute(drillMap);
        source.update((u8 *)colorizer.colorMap.ptr);
        normal.update((u8 *)colorizer.normalMap.ptr);

        colorizer.compute(drillMap2);
        source2.update((u8 *)colorizer.colorMap.ptr);
        normal2.update((u8 *)colorizer.normalMap.ptr);
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
