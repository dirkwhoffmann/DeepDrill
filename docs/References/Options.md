# Configuration Options

DeepDrill is configured by specifying one or more profiles as command line parameters. Each profile contains a certain number of key-value pairs organized in sections. A typical profile looks like this:

```
[image]
width = 960
height = 540

[video]
width = 480
height = 270

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

### Section `[image]`

- `width`
  
  The horizontal image resolution.

- `height` 

  The vertical image resolution.

- `badpixels`

  Percentage of pixels that are allowed to be miscolored.

### Section `[video]`

- `framerate`

  Frames per second of the computed video.

- `width`

  Horizontal video resolution. By default, half of the image resolution is used as video resolution. Doing so ensures that keyframes can be scaled without loss of quality.

- `height`

  Vertical video resolution. By default, half of the image resolution is used as video resolution. Doing so ensures that keyframes can be scaled without loss of quality.

- `inbetweens`

  Number of interpolated frames between two keyframes. By default, this value is assigned twice the frame rate. Consequently, each keyframe is displayed for 2 seconds.

- `palette`

  The color palette. If this value is set to `default`, DeepDrill's standard palette will be used. Alternatively, user-defined color values can be assigned to this value. See profile `vulcano.prf` or `ice.prf` for details.

### Section `[perturbation]`

- `tolerance`

  This value is used by the perturbation algorithm. Please refer to the "Theory" section for details. 

- `rounds`

  This value is used by the perturbation algorithm. Please refer to the "Theory" section for details. 

### Section `[approximation]`

- `coefficients`

  This value is used by the series approximation algorithm. Please refer to the "Theory" section for details. 

- `tolerance`

  This value is used by the series approximation algorithm. Please refer to the "Theory" section for details. 
