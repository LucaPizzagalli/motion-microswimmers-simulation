COMPILER = g++
COMPILER_FLAGS = -std=c++0x -Wall -pedantic -I/usr/local/include -ggdb -O0 -Dusesdl
LINKER_FLAGS = -std=c++0x -lm -lgsl -lgslcblas -L/usr/local/lib -lSDL2 -lSDL2_ttf -ggdb
SOURCES = src/*.cpp
OBJECTS := ${subst src/,,$(SOURCES:.cpp=.o)}

all: bin/simulation

bin/simulation: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(LINKER_FLAGS) -o bin/simulation

$(OBJECTS): $(SOURCES) $(SOURCES:.cpp=hpp)
	$(COMPILER) -c $(SOURCES) $(COMPILER_FLAGS)