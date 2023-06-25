# Adding texture overlays

DeepDrill supports to mix the pixels derived from the color palette with the pixels from a texture image. To enable this feature, simply assign an image file to key `colors.texture`:
```none
./deepdrill -o top.map -o top.jpg top.ini colors.texture=wood.jpg
```
![Textured Mandelbrot Image](images/textured.jpg "Textured Mandelbrot Image")

File `wood.jpg` is located in folder `textures` which is one of the default search locations for asset files. This directory contains some other nice texture you may want to check out.
