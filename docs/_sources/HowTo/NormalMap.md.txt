# Exporting the Normal Map

The creation of spatial images as well as the calculation of texture overlays requires the calculation of a normal map. Normally, DeepDrill calculates this map internally and does not share it with the user. However, the map can be exported by adding the `normalmap.ini` profile to the command line options. The profile tells DeepDrill to run a special GPU filter that converts the normal map vectors into to RGB values:
```INI
[gpu]
illuminator = normalmap.glsl
```
Running DeepDrill with this profile results in the following image: 
```bash
./deepdrill -o normalmap.jpg top.ini normalmap.ini 
```
![Normal Map](images/normalmap.jpg "Normal Map")

The created image shows the typical bluish appearance of normal maps. The colors result from the way the normal vectors are encoded into the color channels of the image. 
