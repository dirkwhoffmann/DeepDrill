# Colorizing Images

In this tutorial you will learn how to recolor images.

By default, DeepDrill creates images with a predefined color palette and other standard settings. Settings can be altered by specifying one or more profiles (`.prf` files) at the command line. 

To create our example image with different colors, we setup a custom profile named `colors.prf` with the following content: 
```
[palette]
colors = vulcano.png
scale = 1.75
```
The first key specifies the color pattern. The second key is a scaling factor that determines how fast the color pattern repeats.

In the next step, we instruct DeepDrill to create a png image from the map file generated in the previous tutorial. Our custom profile is passed to DeepDrill with the `-p` option:
```bash
./deepdrill -v -o spider.png -p colors.prf spider.map
```
Now, the image is now created in the following form: 

![DeepDrill Cover Image](images/spider-vulcano.png "Recolored Cover Image")

As you have seen above, the color palette is specified in the form of an image file. For simplicity, there are no restrictions on the format of the image. DeepDrill composes the color palette from the pixels of the first horizontal line. The horizontal resolution of the palette does not affect the overall appearance of the image. However, wider images will result in slightly smoother color transitions.

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



