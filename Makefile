COMPILER = g++
COMPILER_FLAGS = -c -ggdb -std=c++0x -Wall -pedantic -I/usr/local/include #-O3 for release #-ggdb for debug
LINKER_FLAGS = -ggdb -std=c++0x -L/usr/local/lib -lgsl -lgslcblas -lm -lSDL2 -o #-O3 for release #-ggdb for debug #-lSDL2_image -pthread -o
SOURCES = *.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: simulation

simulation: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(LINKER_FLAGS) simulation

$(OBJECTS): $(SOURCES) $(SOURCES:.cpp=h)
	$(COMPILER) $(COMPILER_FLAGS) $(SOURCES)
