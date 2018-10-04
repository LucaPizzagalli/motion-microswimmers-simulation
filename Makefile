COMPILER = g++
COMPILER_FLAGS = -c -ggdb -std=c++0x -Wall -pedantic -I/usr/local/include #-O3 #-g for debug
LINKER_FLAGS = -std=c++0x -ggdb -L/usr/local/lib -lgsl -lgslcblas -lm -lSDL2 -o #-lSDL2_image -pthread -o
SOURCES = *.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: simulation

simulation: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(LINKER_FLAGS) simulation

$(OBJECTS): $(SOURCES)
	$(COMPILER) $(COMPILER_FLAGS) $(SOURCES)
