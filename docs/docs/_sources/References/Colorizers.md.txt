# Colorizing Algorithms

DeepDrill offers multiple algorithms for colorizing images. 

## Classic Escape-time Coloring 

The classic escape-time coloring algorithm computes an iteration count for each pixel and uses this value as an index into a color table. To select the classic algorithm in DeepDrill, set the `palette.mode` key to `classic`:

```shell
./deepdrill top.ini -o top.jpg palette.mode=classic
```
![Classic escape-time coloring](images/classic.jpg "Classic escape-time coloring")

The image shows the typical color bands that can be seen in many Mandelbrot images, especially older ones. They stem from the fact that the iteration count is an integer value that can only increase in discrete steps.

## Continuous Escape-time Coloring

Color bands can be eliminated by using fractional iteration counts. Please refer to the theory section to learn how the iteration count can be made continuous. To use this coloring method in DeepDrill, set the `palette.mode` key to `smooth`:
```shell
./deepdrill top.ini -o top.jpg palette.mode=smooth
```
![Continuous escape-time coloring](images/smooth.jpg "Continuous escape-time coloring")

## Distance Coloring

Distance coloring is based on the estimated distance of a pixel from the Mandelbrot set. This colorizing technique is often applied for computing black-and-white images because it can lead to very crisp illustrations of the Mandelbrot set. 

To create such images, DeepDrill offers a special GPU shader called `distance.glsl`. It can be activated as follows: 
```shell
./deepdrill seahorse2.ini -o seahorse2.map -o seahorse2.jpg palette.image=white.png gpu.colorizer=distance.glsl distance.threshold=0.2
```
![Distance Coloring](images/distance1.jpg "Distance Coloring")

By using different color options, it is possible to create partially transparent images. E.g., to create the image shown
above with a transparent background, run DeepDrill with the `transparent` palette:
```shell
./deepdrill seahorse2.map -o seahorse2b.png palette.image=transparent.png gpu.colorizer=distance.glsl distance.threshold=0.2
```
![Distance Coloring](images/distance2.png "Distance Coloring (transparent background)")

When adding transparency, make sure to export to a png image since the jpg format and bmp format ignore any alpha channel information.

Alternatively, you may choose to use a transparent color for the Mandelbrot set:
```shell
./deepdrill seahorse2.map -o seahorse2c.png palette.image=white.png palette.bgcolor=transparent gpu.colorizer=distance.glsl distance.threshold=0.2
```
![Distance Coloring](images/distance3.png "Distance Coloring (transparent foreground)")