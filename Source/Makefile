SUBDIRS = Commons DeepDrill DDImage
MYCC = g++ -std=c++17 -O3 -Wall -Wfatal-errors

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
I_GMP = -I /usr/local/Cellar/gmp/6.2.1/include
L_GMP = -L /usr/local/Cellar/gmp/6.2.1/lib
else
I_GMP = -I /usr/local/include
L_GMP = -L /usr/local/lib
endif

export MYCC I_GMP L_GMP

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

clean:
	@echo "Cleaning up $(CURDIR)"
	@rm -f $(EXEC) *.o
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
