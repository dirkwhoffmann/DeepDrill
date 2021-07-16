.PHONY: all clean

SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)

all: $(OBJ)
	@echo > /dev/null

clean:
	@echo "Cleaning up $(CURDIR)"
	@rm -f *.o

%.o: %.cpp $(DEPS)
	@echo "Compiling $<"
	@$(MYCC) $(MYINC) -c -o $@ $<
