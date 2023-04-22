# ------------------------------------------------------------------------------
# This file is part of DeepDrill
#
# A Mandelbrot generator based on perturbation and series approximation
#
# Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
# Licensed under the GNU General Public License v3
#
# See https://www.gnu.org for license information
# ------------------------------------------------------------------------------

UTILDIR    = util
MATHDIR    = math
SHAREDDIR  = shared
DDDIR      = ddrill
DFDIR      = dflight
BINDIR     = ../bin
DEEPDRILL  = $(BINDIR)/deepdrill
DEEPFLIGHT = $(BINDIR)/deepflight
CXX        = g++
CC         = $(CXX)
CCFLAGS    = -std=c++20 -O3 -Wall -Wfatal-errors
CPPFLAGS   = -I$(CURDIR) \
             -I$(CURDIR)/$(UTILDIR) \
			 -I$(CURDIR)/$(MATHDIR) \
			 -I$(CURDIR)/$(SHAREDDIR) \
			 -I$(CURDIR)/$(DDDIR) \
			 -I/usr/local/include \
			 $(shell pkg-config --cflags gmpxx) \
			 $(shell pkg-config --cflags sfml-system sfml-window sfml-graphics)
LDFLAGS    = -pthread \
             -L/usr/local/lib \
             $(shell pkg-config --libs gmpxx) \
			 $(shell pkg-config --libs sfml-system sfml-window sfml-graphics)
SRC        = $(wildcard *.cpp)
MATHSRC    = $(wildcard math/*.cpp)
UTILSRC    = $(wildcard util/*.cpp)
SHAREDSRC  = $(wildcard shared/*.cpp)
DDSRC      = $(wildcard ddrill/*.cpp) $(MATHSRC) $(UTILSRC) $(SHAREDSRC)
DFSRC      = $(wildcard dflight/*.cpp) $(MATHSRC) $(UTILSRC) $(SHAREDSRC)
DDOBJ      = $(DDSRC:.cpp=.o)
DFOBJ      = $(DFSRC:.cpp=.o)

.PHONY: all clean

all: $(DEEPDRILL) $(DEEPFLIGHT)
	@echo > /dev/null

%.o: %.cpp
	@echo "Compiling $<"
	@$(CC) $(CCFLAGS) $(CPPFLAGS) -c -o $@ $<

$(DEEPDRILL): $(DDOBJ)
	@echo "Linking $@"
	@$(CC) $(LDFLAGS) $(DDOBJ) -o $(DEEPDRILL)

$(DEEPFLIGHT): $(MATHOBJ) $(UTILOBJ) $(SHAREDOBJ) $(DFOBJ)
	@echo "Linking $@"
	@$(CC) $(LDFLAGS) $(DFOBJ) -o $(DEEPFLIGHT)

clean:
	@rm -f $(DEEPDRILL) $(DEEPFLIGHT) *.o */*.o
