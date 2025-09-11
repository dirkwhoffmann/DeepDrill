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

| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `real`  | 0.0 | The real part of the image's center coordiate.
| `imag`  | 0.0 | The imaginary part of the image's center coordiate.
| `zoom`  | 1.0 | The magnification level of the image.
| `depth`  | 1.0 | The Maximum number of iterations.
| `escape`  | 1e32 | Once the iterated value gets bigger than this value, the corresponding coordinate is considered to be part of the Mandelbrot set.


### Section `[map]`

| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `width`  | 1920 | The horizontal resolution of the drill map. If this key is not specified, the width of the drill map is automatically chosen to match the image width.
| `height`  | 1080 | The vertical resolution of the drill map. If this key is not specified, the width of the drill map is automatically chosen to match the image height.

### Section `[mapfile]`

| `compress`  | yes | Indicates whether map files should be saved in compressed format.


### Section `[image]`

| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `width`  | 1920 | The horizontal resolution of the image in pixels.
| `height`  | 1080 | The vertical resolution of the image in pixels.



### Section `[video]`

| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `framerate`  | 60 | Frames per second of the computed video. By default, videos with 60 fps are computed to match the frame rate of standard TFTs.
| `keyframes`  | *derived* | Number of keyframes of the final video. If this key is not specified, DeepDrill derives the number from the zoom factor given in the location section.
| `startframe`  | 0 | Number of the keyframe where the video starts. 
| `velocity`  | 1.0 | Zoom speed. The assigned value defines the time interval between two keyframes in seconds.
| `bitrate`  | 8000 | This value is passed directly to the FFmpeg backend. Higher values result in better image quality.


### Section `[palette]`

| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `image`  | - | Path to an image file storing the color palette. DeepDrill will derive the color palette from the uppermost horizontal line of the image.
| `bgcolor`  | black | Background color. This color is used for colorizing pixels belonging to the Mandelbrot set.
| `mode`  | smooth | Coloring mode. Options are `classic` and `smooth`. 
| `scale`  | 1.0 | An optional scaling factor that can be used to stretch or shrink the color palette.  
| `offset`  | -0.15 | An optional offset that can be used to rotate the color palette.  

### Section `[distance]`

| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `threshold`  | 0.0 | This value is used by the distance-estimation filter. A pixel is considered to be part of the Mandelbrot 
set if its distance estimate is below the threshold value. Smaller numbers lead to more fine-grained images. 

### Section `[texture]`
| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `image`  | - | Path to an overlay image. If an overlay image is specified, the derived color value will be mixed with the pixels from the overlay image.
| `opacity`  | 0.5 | The weight factor used to mix color pixels and overlay pixels.
| `scale`  | 1.0 | An optional scaling factor that can be used to stretch or shrink the overlay image.
| `offset`  | 0.0 | An optional offset that can be used to rotate the overlay image.

### Section `[lighting]`
| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `enable`  | no | Indicates whether the normal map should be utilized to create a spatial image.
| `alpha`  | 45 | The direction of the incoming light on the xy plane.
| `beta`  | 45 | The inclination of the light source.  


### Section `[gpu]`
| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `colorizer`  | gradient.glsl | Path to a GLSL shader. The colorizer is the first of three GPU shaders. It utilzes the color palette to derive a RGB value for each pixel.
| `illuminator`  | lambert.glsl | Path to a GLSL shader. The illuminator is the second of three GPU shaders. It applies a lighting model to create a spatial images. This shader is skipped if lighting is disabled. 
| `scaler`  | tricubic.glsl | Path to a GLSL shader. The scaler is the third of three GPU shaders. It scales the internal image down to the final image.


### Section `[perturbation]`

| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `enable`  | yes | Indicates if perturbation should be used to calculate the image. If set to false, DeepDrill falls back to the (very slow) standard algorithm. 
| `tolerance`  | 1e-6 | This value is used by the perturbation algorithm. Please refer to the *Theory* section for details. 
| `badpixels`  | 0.001 | Percentage of pixels that are allowed to be miscolored. 
| `rounds`  | 50 | This value is used by the perturbation algorithm. Please refer to the *Theory* section for details. 
| `color`  | black | Color used for colorizing glitch points.


### Section `[approximation]`

| <div style="width:130px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `enable`  | yes | Indicates if series approximation should be used to calculate the image. If set to false, the iteration always starts at 0. Please refer to the *Theory* section for details.  
| `coefficients`  | 5 | This value is used by the series approximation algorithm. Please refer to the *Theory* section for details. 
| `tolerance`  | 1e-12 | This value is used by the series approximation algorithm. Please refer to the *Theory* section for details. 


### Section `[areachecking]`

| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `enable`  | yes | Specifies whether each pixel is to be tested for belonging to the main bulb or the cartioid. If the test is positive, the pixel is known to belong to the Mandelbrot set without further calculations.
| `color`  | black | Color used for colorizing pixels with a positive area check.


### Section `[attractorcheck]`

| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `enable`  | yes | Specifies whether attractor checking should be carried out. If the test is positive, the pixel is known to belong to the Mandelbrot set without further calculations.
| `tolerance`  | 1e-34 | Tolerance for considering two values as beeing equal. 
| `color`  | black | Color used for colorizing pixels with a positive attractor check.


### Section `[periodcheck]`

| <div style="width:120px">Key</div> | <div style="width:100px">Default value</div> | Description |
|-----|---------|-------------|
| `enable`  | yes | Specifies whether period checking should be carried out. If the test is positive, the pixel is known to belong to the Mandelbrot set without further calculations.
| `tolerance`  | 1e-74 | Tolerance for considering two values as beeing equal. 
| `color`  | black | Color used for colorizing pixels with a positive period check.
