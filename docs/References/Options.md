# Configuration Options

DeepDrill is configured by specifying one or more profiles as command line parameters. Each profile contains a certain number of key-value pairs organized in sections. A typical profile looks like this:

```
[image]
width = 960
height = 540

[perturbation]
tolerance = 1e-6
rounds = 50

[approximation]
coefficients = 5
```

Each option is briefly explained below: 

## Key-value pairs

### Section `[location]`

- `real`

  Real part of the image's center coordiate.

- `imag`

  Imaginary part of the image's center coordiate.

- `zoom`

  The magnification level of the image.

- `depth`

  Maximum number of iterations.

### Section `[map]`

- `width`
  
  The horizontal resolution of the drill map. If this key is not specified, the drill map's width will match the image width.

- `height` 

  The vertical resolution of the drill map. If this key is not specified, the drill map's height will match the image height.

- `depth` 

  Possible values are 0 and 1. If set to 1, a normal map will be written to the `.map` file.

### Section `[image]`

- `width`
  
  The horizontal resolution of the image.

- `height` 

  The vertical resolution of the image.

- `depth` 

  Possible values are 0 and 1. If set to 1, a normal map will be utilized to generate a spatial effect. 

- `illuminator` 

  Path to a GLSL shader. The illumination shader is the first of two GPU shaders. It derives the RGB value for each pixel and applies a lighting model.

- `scaler` 

  Path to a GLSL shader. The scaler shader is the second of two GPU shaders. It scales the internal image down to the final image.

### Section `[video]`

- `framerate`

  Frames per second of the computed video.

- `keyframes`

  Number of keyframes of the final video. If this key is not specified, DeepDrill derives the number of keyframes based on the zoom factor given in the location section.

- `inbetweens`

  Number of interpolated frames between two keyframes. By default, this value is assigned twice the frame rate. Consequently, each keyframe is displayed for 2 seconds.

- `bitrate`
  
  This value is passed to the FFmpeg backend. 

- `scaler` 

  If a video is produced, this downscaler is utilized instead of the one specified in the image section. 

### Section `[colors]`

- `palette`

  Path to an image file storing the color palette. DeepDrill will derive the color palette from the uppermost horizontal line of the image. 

- `scale`

  An optional scaling factor that can be used to stretch or shrink the color palette.  

- `alpha`, `beta`

  These two values define the direction of the light vector. They only take effect if spatial images are computed, i.e., if `depth` is set to 1 in the image section. 

### Section `[perturbation]`

- `enable`

  Indicates if perturbation should be used to calculate the image. If set to false, DeepDrill falls back to the (very slow) standard algorithm. 

- `tolerance`

  This value is used by the perturbation algorithm. Please refer to the "Theory" section for details. 

- `badpixels`

  Percentage of pixels that are allowed to be miscolored.

- `rounds`

  This value is used by the perturbation algorithm. Please refer to the *Theory* section for details. 

### Section `[approximation]`

- `enable`

  Indicates if series approximation should be used to calculate the image. If set to false, the iteration always starts at 0. Please refer to the *Theory* section for details.  

- `coefficients`

  This value is used by the series approximation algorithm. Please refer to the *Theory* section for details. 

- `tolerance`

  This value is used by the series approximation algorithm. Please refer to the *Theory* section for details. 

### Section `[debug]`

- `glitches`

  if set to `true`, glitch points are highlighted in red in the final image. It's main purpose is to debug the glitch detection algorithm.
  