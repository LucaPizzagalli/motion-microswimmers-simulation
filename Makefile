COMPILER = g++
COMPILER_FLAGS = -c -Wall -pedantic #-O3
LINKER_FLAGS = -std=c++0x -lSDL2 -o #-lSDL2_image -pthread -o
SOURCES = *.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: simulation

simulation: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(LINKER_FLAGS) simulation

$(OBJECTS): $(SOURCES)
	$(COMPILER) $(COMPILER_FLAGS) $(SOURCES)
