# Computing spatial images

DeepDrill supports the creation of spatial images. The easiest way to add a 3D effect in DeepDrill is to pass in the `3D.ini` profile which defines three key-value pairs:
```INI
[image]
depth = 1

[colors]
alpha = 45
beta = 65
```
Setting `image.depth` to `1` instructs DeepDrill to mix the derived pixel color values with depth information from a normal map. Running DeepDrill with this profile results in the following image:
```none
./deepdrill -o top.map -o top.jpg 3D.ini top.ini
```
![3D Mandelbrot Image](images/3D_1.jpg "3D Mandelbrot Image")

The latter two key-value pairs define the direction of the light vector. `alpha` specifies the direction of the incoming light on the xy plane, and `beta` is the inclination of the light source. Decreasing this angle increases the 3D effect:
```none
./deepdrill -o top.jpg 3D.ini top.map colors.beta=30
```
![3D Mandelbrot Image](images/3D_2.jpg "3D Mandelbrot Image")

Another predefined profile for computing spatial images is `relief.ini`. In addition to the options of the 3D profile, it selects a special GPU filter called `relief.glsl`. This filter ignores the color information and converts the normal map directly into a grayscale image:
```INI
[image]
illuminator = relief.glsl
depth = 1

[colors]
alpha = 45
beta = 65
```
Running DeepDrill with this profile yields the following result:
```bash
./deepdrill -o top.jpg relief.ini top.map
```
![Mandelbrot Relief](images/relief.jpg "Mandelbrot Relief")
