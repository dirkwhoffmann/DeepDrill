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

#pragma once

auto bypassShader =
"uniform sampler2D texture;                                                 "
"                                                                           "
"void main()                                                                "
"{                                                                          "
"    vec2 coord = gl_TexCoord[0].xy;                                        "
"    vec4 pixel = texture2D(texture, coord);                                "
"    gl_FragColor = gl_Color * pixel;                                       "
"}                                                                          ";

auto mergeShader =
"uniform sampler2D current;                                                 "
"uniform sampler2D prev;                                                    "
"uniform sampler2D prevprev;                                                "
"                                                                           "
"void main()                                                                "
"{                                                                          "
"    vec2 coord = gl_TexCoord[0].xy;                                        "
"    vec4 pixel = texture2D(current, coord);                                "
"    gl_FragColor = gl_Color * pixel;                                       "
"}                                                                          ";
