# Configuration Options

DeepDrill is configured by specifying one or more profiles as command line parameters. Each profile contains a certain number of key-value pairs organized in sections. A typical profile looks like this:

```INI
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

| <div style="width:100px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `real`  | 0.0 | The real part of the image's center coordiate.
| `imag`  | 0.0 | The imaginary part of the image's center coordiate.
| `zoom`  | 1.0 | The magnification level of the image.
| `depth`  | 1.0 | The Maximum number of iterations.


### Section `[map]`

| <div style="width:100px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `width`  | 1920 | The horizontal resolution of the drill map. If this key is not specified, the width of the drill map is automatically chosen to match the image width.
| `height`  | 1080 | The vertical resolution of the drill map. If this key is not specified, the width of the drill map is automatically chosen to match the image height.
| `depth`  | 1 | Possible values are 0 and 1. If set to 1, a normal map will be written to the `.map` file.
| `compress`  | yes | Indicates whether map files should be saved in compressed format.


### Section `[image]`

| <div style="width:100px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `width`  | 1920 | The horizontal resolution of the image.
| `height`  | 1080 | The vertical resolution of the image.
| `depth`  | 1 | Possible values are 0 and 1. If set to 1, a spatial effect will be generated from the normal map. 
| `illuminator`  | lambert.glsl | Path to a GLSL shader. The illuminator is the first of two GPU shaders. It derives the RGB value for each pixel and applies a lighting model.
| `scaler`  | bicubic.glsl | Path to a GLSL shader. The scaler is the second of two GPU shaders. It scales the internal image down to the final image.


### Section `[video]`

| <div style="width:100px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `framerate`  | 60 | Frames per second of the computed video. By default, videos with 60 fps are computed to match the frame rate of standard TFTs.
| `keyframes`  | *derived* | Number of keyframes of the final video. If this key is not specified, DeepDrill derives the number from the zoom factor given in the location section.
| `inbetweens`  | *derived* | Number of interpolated frames between two keyframes. By default, this value is assigned twice the frame rate. Consequently, each keyframe is displayed for 2 seconds.
| `bitrate`  | 8000 | This value is passed directly to the FFmpeg backend. Higher values result in better image quality.
| `scaler`  | tricubic.glsl | Path to a GLSL shader. If a video is created, this downscaler is used instead of the one specified in the image section. 


### Section `[colors]`

| <div style="width:100px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `palette`  | - | Path to an image file storing the color palette. DeepDrill will derive the color palette from the uppermost horizontal line of the image.
| `texture`  | - | Path to an overlay image. If an overlay image is specified, the derived color value will be mixed with the pixels from the overlay image.
| `scale`  | 1.0 | An optional scaling factor that can be used to stretch or shrink the color palette.  
| `opacity`  | 0.5 | The weight factor used to mix color pixels and overlay pixels.
| `alpha`  | 45 | The direction of the incoming light on the xy plane.
| `beta`  | 45 | The inclination of the light source.  


### Section `[perturbation]`

| <div style="width:100px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `enable`  | yes | Indicates if perturbation should be used to calculate the image. If set to false, DeepDrill falls back to the (very slow) standard algorithm. 
| `tolerance`  | 1e-6 | This value is used by the perturbation algorithm. Please refer to the *Theory* section for details. 
| `badpixels`  | 0.001 | Percentage of pixels that are allowed to be miscolored. 
| `rounds`  | 50 | This value is used by the perturbation algorithm. Please refer to the *Theory* section for details. 
| `color`  | black | Color used for colorizing glitch points.


### Section `[approximation]`

| <div style="width:100px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `enable`  | yes | Indicates if series approximation should be used to calculate the image. If set to false, the iteration always starts at 0. Please refer to the *Theory* section for details.  
| `coefficients`  | 5 | This value is used by the series approximation algorithm. Please refer to the *Theory* section for details. 
| `tolerance`  | 1e-12 | This value is used by the series approximation algorithm. Please refer to the *Theory* section for details. 


### Section `[areachecking]`

| <div style="width:100px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `enable`  | yes | Specifies whether each pixel is to be tested for belonging to the main bulb or the cartioid. If the test is positive, the pixel is known to belong to the Mandelbrot set without further calculations.
| `color`  | black | Color used for colorizing pixels with a positive area check.


### Section `[attractorcheck]`

| <div style="width:100px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `enable`  | yes | Specifies whether attractor checking should be carried out. If the test is positive, the pixel is known to belong to the Mandelbrot set without further calculations.
| `tolerance`  | 1e-34 | Tolerance for considering two values as beeing equal. 
| `color`  | black | Color used for colorizing pixels with a positive attractor check.


### Section `[periodcheck]`

| <div style="width:100px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `enable`  | yes | Specifies whether period checking should be carried out. If the test is positive, the pixel is known to belong to the Mandelbrot set without further calculations.
| `tolerance`  | 1e-74 | Tolerance for considering two values as beeing equal. 
| `color`  | black | Color used for colorizing pixels with a positive period check.
