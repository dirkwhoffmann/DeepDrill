# Computing Zoom Videos

In this tutorial, you'll learn how to create Mandelbrot zoom videos using the DeepDrill tool chain. In particular, we are going to compute the following two videos that are available on Youtube: 

<p float="left">
    <a href = "https://www.youtube.com/watch?v=Ph3vJASDVaE"><img src="https://dirkwhoffmann.github.io/DeepDrill/images/spider-youtube.png" width="49%"></a>
    <a href = "https://www.youtube.com/watch?v=OcXcHgwJ33g"><img src="https://dirkwhoffmann.github.io/DeepDrill/images/infinity-youtube.png" width="49%"></a>
</p>

Unlike calculating still images, creating a zoom video requires a much more sophisticated workflow. For example, multiple keyframes must be calculated and stitched together in the right way. To simplify the process, video creation has been divided into three phases. In the first phase, DeepDrill is used to create a Makefile and a larger number of input files. In the second phase, the Makefile is executed to compute all keyframe images. In the last phase, DeepFlight is utilized to assemble the final video by calculating intermediate frames.

## Setting up the workflow

Out first task is to create an empty project directory:
```bash
mkdir project
```
To set up a workflow in this directory, we start DeepDrill with the `-m` option:
```bash
./deepdrill -m -p ../tutorial/spider.prf -o project ../tutorial/spider.loc
```
```
DeepDrill 1.0 - (C)opyright Dirk W. Hoffmann

     Generating project file: ................................. 0.00 sec
   Generating location files: ................................. 0.04 sec
          Generating profile: ................................. 0.00 sec
         Generating Makefile: ................................. 0.00 sec

Total time: 0.05 sec
```
If no profile had been specified, DeepDrill would have set up the workflow to zoom to the location specified in `spider.loc` with predefined settings. In our example, this would result in a fairly long video. To customize settings, we instructed DeepDrill to read in `spider.prf` before setting up the workflow. This profile defines two keys:
```
[image]
width = 3840
height = 2160

[video]
keyframes = 75
inbetweens = 143
bitrate = 8000

[perturbation]
tolerance = 1e-6
rounds = 100

[approximation]
coefficients = 5
```
The first two keys specify the resolution of the keyframe images. The resolution of the generated video will be half the size. Hence, a video with a standard HD resolution of 1920 x 1080 pixels will be produced.  

The next key specifies the number of keyframes to calculate, and the third key specifies how many frames to insert between two consecutive keyframes. If the video was recorded at 60 Hz, a value of 60 results in a delay of one second between two keyframes. In our case, a value of 143 is given. This means that it takes a little more than 2 seconds to get from one keyframe to the next. The result is a video with a total length of about 3 minutes. 

Let's take a closer look at the files DeepDrill created in the project directory:  
- `spider.prj`
  This file contains several key-value pairs which will be picked up later by `deepflight` to assemble the final video.

- `spider.prf`
  This profile is used for calculating keyframes. It is composed out of the profiles that were passed in as command line arguments when the workflow was set up.

- `spider_0.loc` to `spider_74.loc`
  These position files contain the coordinates and zoom factors for each keyframe.

- `Makefile`
  This is a standard Makefile that automatically calculates all keyframe images. In the next section we will take a closer look at its contents.

## Computing all keyframes

For our tutorial project, DeepDrill has created the following Makefile:

```Make
DEEPDRILL  = /Users/hoff/Retro/DeepDrill/bin/./deepdrill
DEEPFLIGHT = /Users/hoff/Retro/DeepDrill/bin/./deepflight
MAPS       = $(patsubst %.loc,%.map,$(wildcard *_*.loc))
IMAGES     = $(patsubst %.loc,%.png,$(wildcard *_*.loc))
VIDEO      = spider.mov
NUM_IMAGES = $(words $(IMAGES))
MAPFLAGS   = -b
PNGFLAGS   = -b

.PHONY: all clean

all: $(IMAGES) $(MAPS)

%.map: %.loc
        @$(DEEPDRILL) $(MAPFLAGS) -p spider.prf -o $*.map $*.loc > $*.progress
        @mv $*.progress $*.log

%.png: %.map
        @$(DEEPDRILL) $(PNGFLAGS) -p spider.prf -o $*.png $*.map > /dev/null

$(VIDEO): $(IMAGES)
        @$(DEEPFLIGHT) $(MOVFLAGS) spider.prj -o $(VIDEO)
clean:
        rm *.mov *.map *.png *.log
```
The Makefile defines two major goals: One is to create the map files from the location files and the other is to convert the map files into images. To create the images, we simply change to the project directory and run the Make utility:
```bash
cd project
make -j
```
The `-j` option is optional. If it is specified, the Make utility runs several jobs in parallel, which significantly reduces total calculation time.

Depending on the performance of your machine, it may take a while to calculate all images. After successful completion, the project directory contains a map file and an image file for each keyframe. 

## Assembling the zoom video

The final step is to combine all the keyframes into a zoom video by calling `make` with the `spider.mov` target as argument:
```bash
make spider.mov 
```
This target executes a separate tool called `deepflight` which stiches together all previously computed keyframes. After completion, the file `spider.mov was created, which contains the final video:
[![Youtube screenshot](images/spider-youtube.png)](https://www.youtube.com/watch?v=Ph3vJASDVaE)

## Creating the second video

The second tutorial video can be created with the same workflow. You'll find the corresponding location file in `tutorial/infinity.loc` and a predefined profile in `tutorial/infinity.prf`. The second video is about three times as long as the first one and many keyframes need more computation time. On an avarage machine, it may take a few hours to create the entire video.

[![Youtube screenshot](images/infinity-youtube.png)](https://www.youtube.com/watch?v=OcXcHgwJ33g)
