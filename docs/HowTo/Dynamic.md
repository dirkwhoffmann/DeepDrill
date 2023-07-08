# Working with Dynamic Values

As you've already learned, DeepDrill is configured by defining several key-value pairs. E.g., by setting 
```INI
[video]
velocity = 0.5
```
we advice DeepZoom to zoom in at a pace of 2 seconds per keyframe. The velocity won't change throughout the entire video.

However, `velocity` is a so-called dynamic value which means that it can be varied throughout the video. This is achieved by assigning a split description instead of fixed value. A spline is defined by several $x/y$ pairs where $x$ is a time stamp and $y$ is the value we want to assign at that point in time.

For the following example, we assume that you've worked through the zoom-video tutorial. I.e., we assume that `project` contains all precomputed keyframes for the video. Then, the zoom velocity can be made dynamic like this:
```
./deepzoom project video.velocity=0:0/0,0:2/1,0:4/-1,0:6/2,0:8/-2,0:10/3,0:12/-3,0:14/0,0:16/0 video.startframe=30 -o ~/Desktop/wobble.mov
```
This is the result: 

<iframe width="560" height="315" src="https://www.youtube.com/embed/gM5hYQWW9Bk" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>

The following keys refer to dynamic values: 

- `video.velocity`
- `palette.scale`
- `palette.offset`
- `texture.opacity`
- `texture.scale`
- `texture.offset`
- `lighting.alpha`
- `lighting.beta`

