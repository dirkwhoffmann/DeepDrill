SUBDIRS = util math
MYINC = -I$(CURDIR) -I$(CURDIR)/math -I$(CURDIR)/util
MYCC = g++ $(MYINC) -std=c++17 -O3 -Wall -Wfatal-errors

SRC=$(wildcard *.cpp)
MATHSRC=$(wildcard math/*.cpp)
UTILSRC=$(wildcard math/*.cpp)

OBJ=$(SRC:.cpp=.o)
MATHOBJ=$(MATHSRC:.cpp=.o)
UTILOBJ=$(UTILSRC:.cpp=.o)

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
I_GMP = -I /usr/local/Cellar/gmp/6.2.1/include
L_GMP = -L /usr/local/Cellar/gmp/6.2.1/lib
else
I_GMP = -I /usr/local/include
L_GMP = -L /usr/local/lib
endif

export MYCC MYINC I_GMP L_GMP

.PHONY: all prebuild subdirs clean

all: prebuild subdirs
	@echo > /dev/null
	
prebuild:
	@echo "Entering ${CURDIR}"
		
subdirs:
	@for dir in $(SUBDIRS); do \
		echo "Entering ${CURDIR}/$$dir"; \
		$(MAKE) -C $$dir; \
	done

deepdrill: $(OBJ) $(UTILOBJ) $(MATHOBJ)
	@echo "Linking"
	@g++ -pthread -o deepdrill */*.o *.o $(MYLPATH) -lgmpxx -lgmp

clean:
	@echo "Cleaning up $(CURDIR)"
	@rm -f $(EXEC) *.o
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
