EXEC = deepdrill
SUBDIRS = Math
MYCC = g++ -std=c++17 -O3 -Wfatal-errors

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)

MYFLAGS = \
-Wall \
-I $(CURDIR) \
-I $(CURDIR)/Math \
-I $(CURDIR)/../Utilities \
-I /usr/local/Cellar/gmp/6.2.1/include

MYLPATH = \
-L /usr/local/Cellar/gmp/6.2.1/lib

else

MYFLAGS = \
-Wall \
-I $(CURDIR) \
-I $(CURDIR)/Math \
-I $(CURDIR)/Utilities \
-I /usr/local/include

MYLPATH = \
-L /usr/local/lib

endif

export MYCC MYFLAGS

SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)

.PHONY: all prebuild subdirs clean bin

all: prebuild subdirs $(OBJ) bin
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

%.o: %.cpp $(DEPS)
	@echo "Compiling $<"
	@$(MYCC) $(MYFLAGS) -c -o $@ $<

bin:
	@echo "Linking"
	@g++ -pthread -o $(EXEC) *.o */*.o $(MYLPATH) -lgmpxx -lgmp
