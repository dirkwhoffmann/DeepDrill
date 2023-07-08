# Computing Still Images

In this tutorial, you will learn how to create still images using the DeepDrill toolchain. 

## Computing the Mandelbrot Set

We start by computing a top view of the entire Mandelbrot set. To compute the image, we need to tell DeepDrill the location of the object. This is done by passing a configuration file as a command line parameter. All configuration files carry the .ini extension because DeepDrill uses a structured key-value syntax for customization. A predefined location file for the title image is stored in the `locations` directory under `top.ini`. Before we continue, let's peek inside this file:
```INI
[location]
real = -0.7
imag = 0.0
zoom = 1.7
depth = 1000
```
Four key-value pairs are defined. The first two define the position of the center coordinate on the complex number plane. The third pair defines the magnification factor and the fourth pair defines the maximum number of iterations. If an orbit has not yet broken out at the time this iteration is performed, the corresponding point is considered part of the Mandelbrot set.

Assuming the current directory is the build directory, we can start DeepDrill by executing the following command at the command line:
```shell
./deepdrill -o image.map top.ini
```
The following output is generated: 
````none
DeepCDrill 3.1 - (C)opyright Dirk W. Hoffmann

        Running the area check: ................................. 1.09 sec

Round 1: 1784039 points remaining

     Computing reference orbit: ................................. 0.00 sec
        Computing coefficients: ................................. 0.00 sec
         Checking probe points: ................................. 0.00 sec
        Computing delta orbits: ................................. 1.47 sec

All rounds completed: 0 unresolved

            Preparing map file: ................................. 0.05 sec
          Compressing map file: ................................. 0.62 sec
               Saving map file: ................................. 0.00 sec

Total time: 3.31 sec

````
Note that we have not generated the image directly. Instead, we computed a so-called map file, which stores various information about the orbit of each pixel. The creation of the map file is the first phase of a two-step process. In the second phase, the map file is converted into an image. This is done by specifying a map file as input and an image file as output.
```shell
./deepdrill -o image.jpg image.map
```
The following output is generated: 
```none
DeepCDrill 3.1 - (C)opyright Dirk W. Hoffmann

              Loading map file: ................................. 0.00 sec
        Uncompressing map file: ................................. 0.09 sec
           Extracting channels: ................................. 0.03 sec
           Running GPU shaders: ................................. 0.02 sec
                  Saving image: ................................. 0.02 sec

Total time: 0.25 sec
```
Besides JPG images, DeepDrill also supports images of type BMP and PNG.

By default, DeepDrill creates a 1920 x 1080 image with a predefined color palette and other default settings:

![Mandelbrot Image](images/still1.jpg "The Mandelbrot set")

## Customizing the Computation

All settings can be customized by specifying one or more key-value pairs. The simplest way to define such a pair is to pass it in as a command line parameter. For example, the following invocation instructs DeepDrill to create the image in 4K resolution:
```shell
./deepdrill -o top.jpg image.width=3840 image.height=2160 top.ini
```
This example also demonstrates the possibility of going directly from a location file to an image. In this case the map file is only created internally and not written to disk. Since rendering the map file is by far the most time-consuming part, saving map files to disk is the preferred method if a single map file is to be rendered multiple times, e.g. with different color palettes. 

Another way of passing in key-value pairs is by putting the keys in a seperare configuration file. E.g., the `profiles` folder contains a predefined profile named `4K.ini` with the following content:
```INI
[image]
width = 3840
height = 2160
```
DeepDrill accepts any number of configuration files as input. This means that we can simplify the calculation of our 4K image with the following command: 
```shell
./deepdrill -o top.jpg 4K.ini top.ini
```
Note that the old value of a key-value pair is overwritten if it is specified multiple times. All configuration files are processed in the same order as they appear in the argument list.

## Recoloring the Image

Next, we want to compute the Mandelbrot image with a different color palette. In this example, we use the `vulcano` palette which one several predefined palettes. 
```shell
./deepdrill top.map -o image.jpg palette.image=vulcano.png
```
Now, the following image is produced: 

![Recolored Mandelbrot image](images/still2.jpg "Recolored Mandelbrot set")

## Working with Overlays

In addition to colorizing pixels with the colors from a palette image, DeepDrill is able to superimpose the result with an overlay texture. In the following example, we use the 'elvish' image which is one of several predefined overlay textures. 
```shell
./deepdrill top.map -o image.jpg palette.image=vulcano.png texture.image=elvish.jpg 
```
This produces the following result: 

![Textured Mandelbrot image](images/still3.jpg "Textured Mandelbrot set")
