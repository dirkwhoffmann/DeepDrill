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

EXEC       = deepdrill
BINDIR     = ../bin
UTILDIR    = util
MATHDIR    = math
SUBDIRS    = $(UTILDIR) $(MATHDIR)
CXX        = g++
CC         = $(CXX)
CCFLAGS    = -std=c++17 -O3 -Wall -Wfatal-errors
CPPFLAGS   = -I$(CURDIR) \
             -I$(CURDIR)/$(UTILDIR) \
			 -I$(CURDIR)/$(MATHDIR) \
			 -I/usr/local/include \
			 $(shell pkg-config --cflags gmpxx)
LDFLAGS    = -pthread \
             -L/usr/local/lib \
             $(shell pkg-config --libs gmpxx)
SRC        = $(wildcard *.cpp)
MATHSRC    = $(wildcard math/*.cpp)
UTILSRC    = $(wildcard math/*.cpp)
OBJ        = $(SRC:.cpp=.o)
MATHOBJ    = $(MATHSRC:.cpp=.o)
UTILOBJ    = $(UTILSRC:.cpp=.o)

export CC CCFLAGS CPPFLAGS LDFLAGS

.PHONY: all prebuild subdirs clean

all: prebuild subdirs $(OBJ) deepdrill
	@echo > /dev/null
	
prebuild:
	@echo "Entering ${CURDIR}"
		
subdirs:
	@for dir in $(SUBDIRS); do \
		echo "Entering ${CURDIR}/$$dir"; \
		$(MAKE) -C $$dir; \
	done

%.o: %.cpp $(DEPS)
	@echo "Compiling $<"
	$(CC) $(CCFLAGS) $(CPPFLAGS) -c -o $@ $<

$(EXEC): $(OBJ) $(UTILOBJ) $(MATHOBJ)
	@echo "Linking"
	g++ -pthread -o deepdrill */*.o *.o $(MYLPATH) -lgmpxx -lgmp
	# $(CC) $(LDFLAGS) -o $(EXEC) */*.o *.o
	@echo "Installing"
	mv $(EXEC) $(BINDIR)

clean:
	@echo "Cleaning up $(CURDIR)"
	@rm -f $(EXEC) *.o
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
