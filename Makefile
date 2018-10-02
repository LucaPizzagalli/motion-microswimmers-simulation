COMPILER = g++
COMPILERFLAGS = -c -Wall -pedantic #-O3
LINKERFLAGS = -o #-lSDL2 -lSDL2_image -pthread
SOURCES = *.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: simulation

simulation: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(LINKERFLAGS) simulation

$(OBJECTS): $(SOURCES)
	$(COMPILER) $(COMPILERFLAGS) $(SOURCES)
