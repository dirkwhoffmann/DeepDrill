# Adding an Overlay Image

DeepDrill supports to mix the pixels derived from the color palette with the pixels from a texture image. To enable this feature, simply assign an image file to key `texture.image`:
```none
./deepdrill top.ini -o top.jpg texture.image=giraffe.jpg
```
![Textured Mandelbrot Image](images/textured.jpg "Textured Mandelbrot Image")

File `giraffe.jpg` is located in folder `textures` which is one of the default search locations for asset files. This directory contains some other nice texture you may want to check out.
