# Computing Zoom Videos

In this tutorial, you'll learn how to create a Mandelbrot zoom video using the DeepDrill tool chain.

Unlike calculating still images, creating a zoom video requires a much more sophisticated workflow. For example, multiple keyframes must be calculated and stitched together in the right way. To simplify the process, video creation has been divided into three phases. In the first phase, DeepDrill is used to create a Makefile and a larger number of input files. In the second phase, the Makefile is executed to compute all keyframe images. In the last phase, DeepFlight is utilized to assemble the final video by calculating intermediate frames.

## Setting up the workflow

Out first task is to create an empty project directory:
```bash
mkdir project
```
To set up a workflow in this directory, we start DeepDrill with the `-m` option:
```bash
./deepdrill -m -p ../profiles/tutorial.prf -o project ../locations/spider.loc
```
```
DeepDrill 0.9 - (C)opyright Dirk W. Hoffmann

     Generating project file: ................................. 0.00 sec
   Generating location files: ................................. 0.04 sec
          Generating profile: ................................. 0.00 sec
         Generating Makefile: ................................. 0.00 sec

Total time: 0.05 sec
```
If no profile had been specified, DeepDrill would have set up the workflow to zoom to the location specified in `spider.loc`. In our example, this would result in a fairly long video. To shorten the video, we instructed DeepDrill to read in `tutorial.prf` before setting up the workflow. This profile defines two keys:
```
[video]
keyframes = 75
inbetweens = 143
```
The first key specifies the number of keyframes to calculate. The second key specifies how many frames to insert between two consecutive keyframes. If the video was recorded at 60 Hz, a value of 60 results in a delay of one second between two keyframes. In our case, a value of 143 is given. This means that it takes a little more than 2 seconds to get from one keyframe to the next. The result is a video with a total length of about 3 minutes.

Let's take a closer look at the files DeepDrill created in the project directory:  
- `spider.prj`
  This file contains several key-value pairs which will be picked up later by `deepflight` to assemble the final video.

- `spider.prf`
  This profile is used for calculating keyframes. It is composed out of the profiles that were passed in as command line arguments when the workflow was set up.

- `spider_0.loc` to `spider_73.loc`
  These position files contain the coordinates and zoom factors for each keyframe.

- `Makefile`
  This is a standard Makefile that automatically calculates all keyframe images. In the next section we will take a closer look at its contents.

## Computing all keyframes

For our tutorial project, DeepDrill has created the following Makefile:

```Make
DEEPDRILL  = /Users/hoff/Retro/DeepDrill/build/./deepdrill
MAPS       = $(patsubst %.loc,%.map,$(wildcard *_*.loc))
IMAGES     = $(patsubst %.loc,%.png,$(wildcard *_*.loc))
NUM_IMAGES = $(words $(IMAGES))
MAPFLAGS   = -b -v
PNGFLAGS   = -b

.PHONY: all clean

all: $(IMAGES) $(MAPS)
        @echo ""

%.png: %.map
        @$(DEEPDRILL) $(PNGFLAGS) -p spider.prf -o $*.png $*.map

%.map: %.loc
        @$(DEEPDRILL) $(MAPFLAGS) -p spider.prf -o $*.map $*.loc
```
The Makefile defines two major goals: One is to create the map files from the location files and the other is to convert the map files into images. To create the images, we simply change to the project directory and run the Make utility:
```bash
cd project
make -j
```
The `-j` option is optional. If it is specified, the Make utility runs several jobs in parallel, which significantly reduces total calculation time.

Depending on the performance of your machine, it may take a while to calculate all images. After successful completion, the project directory contains a map file and an image file for each keyframe. 

## Assembling the zoom video

The final step is to combine all the keyframes into a zoom video. For this purpose we use a separate tool called `deepflight`. We call this tool with the created project file `spider.prj` as input and a `.mov` file as output:
```
./deepflight -o spider.mov spider.prj
```
After completion, the file `spider.mov was created, which contains the final video.
![Spider movie screenshot](images/spider_mov.png "Spider Movie Screenshot")