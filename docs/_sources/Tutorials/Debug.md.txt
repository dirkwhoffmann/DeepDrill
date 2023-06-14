# Debugging the Toolchain

In this tutorial you will learn how to get detailed information about the calculation process while creating images or videos. 

## Generating Verbose Output

The easiest way to get more information about the currently performed computation is to specify the `--verbose` (`-v`) option on the command line. For example, if we repeat the calculation of the Mandelbrot set that we performed before, we will get the following, more informative output: 
```none
./deepdrill -v top.loc -o top.map

DeepDrill 2.0 - (C)opyright Dirk W. Hoffmann

                        Center: -0.7 + 0i
                   Upper left: (-2.7915,-1.17647)
                  Lower right: (1.38932,1.17429)
                 Magnification: 0.85b1
                      Map size: 1920 x 1080 (3D)
                    Image size: 1920 x 1080 (2D)
                 GMP Precision: 128 Bit

Round 1 / 50: 2073600 points

     Computing reference orbit: ................................. 0.00 sec

               Reference point: (960,540)
        Perturbation tolerance: 1e-06
                 Maximum depth: 1000
                  Actual depth: 1000

        Computing coefficients: ................................. 0.00 sec

                  Coefficients: 5
       Approximation tolerance: 1e-12

         Checking probe points: ................................. 0.00 sec

          Skippable iterations: 0

        Computing delta orbits: ................................. 6.23 sec

                      Glitches: 0

               Saving map file: ................................. 0.07 sec

                      Map size: 1920 x 1080
              Iteration counts: Saved
                      Lognorms: Saved
                   Derivatives: Not saved
                       Normals: Saved

Total time: 6.38 sec
```

## Highlighting Glitch Points 

To enable additional debug options, you may pass in profile `debug.prf`. This profile contains the following definitions:
```INI
[debug]
glitches = yes
```
By setting `debug.glitches` to `true`, DeepDrill will colorize all glitch points in red. For more information about glitch points, please refer to the mathematical background section.

At the time of writing, the `glitches` option is the only available option in the `debug` section. Other options are likely to come in the future as the toolchain matures. 

## Exporting Normal Maps

To create 3D effects, DeepDrill calculates a normal map internally. This map can be made visible by applying the `normalmap.prf` profile. The profile instructs DeepDrill to use a special GPU filter that converts the normal map data into RGB values:
```INI
[image]
depth = 1
illuminator = normalmap.glsl
```
Running DeepDrill with this profile results in the following image: 
```bash
./deepdrill -v -o normalmap.jpg -p normalmap.prf top.loc
```
![Normal Map](images/normalmap.jpg "Normal Map")
