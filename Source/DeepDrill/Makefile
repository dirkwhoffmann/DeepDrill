MYFLAGS = -I$(CURDIR) -I$(CURDIR)/Math -I$(CURDIR)/.. -I$(CURDIR)/../Commons
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)

.PHONY: all prebuild clean bin

all: prebuild $(OBJ) bin
	@echo > /dev/null
	
prebuild:
	@echo "Entering ${CURDIR}"

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
	@g++ -pthread -o deepdrill ../Commons/*.o *.o */*.o $(MYLPATH) -lgmpxx -lgmp
