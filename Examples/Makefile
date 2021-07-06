# Configuration options
EXEC = ../DeepDrill/deepdrill

PROFILES = ../Profiles/thumbnail.ini ../Profiles/debug.ini
#PROFILES = ../Profiles/low.ini

PALETTE = ../Palettes/vulcano.ini

INI=$(wildcard *.ini)
TIFF=$(SRC:.ini=.tiff)

.PHONY: all clean

all: $(TIFF)
	@echo > /dev/null
			
clean:
	@echo "Cleaning up $(CURDIR)"
	@rm -f $(EXEC) *.tiff

%.tiff: %.ini
	$(EXEC) $(PROFILES) $(PALETTE) $<
