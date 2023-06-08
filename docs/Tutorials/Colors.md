# Colorizing Images

In this tutorial you will learn how to recolor images.

By default, DeepDrill creates images with a predefined color palette. To change the color palette, we instruct DeepDrill to read in a custom profile called `tutorial1.prf`:
```bash
./deepdrill -o image.jpg -p tutorial1.prf image.map
```
The contents of this profile looks like this:
```
[colors]
palette = vulcano.png
scale = 3.0
```
Now, the image is now created in the following form: 

![Recolored Mandelbrot set](images/tutorial2.jpg "Recolored Mandelbrot set")

As it was shown above, the color palette is specified in the form of an image file. For simplicity, there are no restrictions on the format of the image. DeepDrill composes the color palette from the pixels of the first horizontal line. The horizontal resolution of the palette does not affect the overall appearance of the image. However, wider images will result in slightly smoother color transitions.

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



