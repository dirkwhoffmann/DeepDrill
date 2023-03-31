
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

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

.PHONY: all clean

all: $(OBJ)
	@echo > /dev/null

clean:
	@echo "Cleaning up $(CURDIR)"
	@rm -f *.o

%.o: %.cpp $(DEPS)
	@echo "Compiling $<"
	$(CC) $(CCFLAGS) $(CPPFLAGS) -c -o $@ $<
