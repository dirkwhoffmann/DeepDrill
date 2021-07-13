SUBDIRS =
MYFLAGS = -I$(CURDIR) -I$(CURDIR)/.. -I$(CURDIR)/../Commons
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
	@g++ -pthread -o ddimage ../Commons/*.o *.o
