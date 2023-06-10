# Colorizing Images

In this tutorial you will learn how to recolor images.

By default, images are created with a predefined color palette. To following call instructs DeepDrill to apply different coloring:
```bash
./deepdrill image.map -c colors.palette=vulcano.png -c colors.scale=3.0 -o image.jpg 
```
This results in the following image: 

![Recolored Mandelbrot set](images/recolored1.jpg "Recolored Mandelbrot set")

As it was shown above, the color palette is specified in the form of an image file. For simplicity, there are no restrictions on the format of the image. DeepDrill composes the color palette from the pixels of the first horizontal line. The horizontal resolution of the palette does not affect the overall appearance of the image. However, wider images will result in slightly smoother color transitions.

In addition, DeepDrill provides a key called `colors.scale` which controls how fast the palette repeats. The following examples shows the effect: 

![Recolored Mandelbrot set](images/recolored2.jpg "Recolored Mandelbrot set")

## Predefined palettes

DeepDrill offers the following predefined color palettes: 

*default*
![Default palette](images/default.png "Default palette")

*candy*
![candy palette](images/candy.png "candy palette")

*dolomiti*
![dolomiti palette](images/dolomiti.png "dolomiti palette")

*elements*
![elements palette](images/elements.png "elements palette")

*ice*
![ice palette](images/ice.png "ice palette")

*lindor*
![lindor palette](images/lindor.png "lindor palette")

*pistachio*
![pistachio palette](images/pistachio.png "pistachio palette")

*prism*
![prism palette](images/prism.png "prism palette")

*ruby*
![ruby palette](images/ruby.png "ruby palette")

*ultrafrac*
![ultrafrac palette](images/ultrafrac.png "ultrafrac palette")

*vulcano*
![vulcano palette](images/vulcano.png "vulcano palette")

The following example show a recolored version of one the images that were computed in the previous tutorial. 

```bash 
./deepdrill seahorse1.loc -o image.jpg -o seahorse1.map -c colors.palette=ruby.png
```
![prism recolored](images/recolored3.jpg "Recolored")
```bash 
./deepdrill seahorse1.map -o image.jpg -c colors.palette=lindor.png
```
![prism recolored](images/recolored4.jpg "Recolored")
```bash 
./deepdrill seahorse1.map -o image.jpg -c colors.palette=dolomiti.png
```
![prism recolored](images/recolored5.jpg "Recolored")
```bash 
./deepdrill seahorse1.map -o image.jpg -c colors.palette=ice.png
```
![prism recolored](images/recolored6.jpg "Recolored")

