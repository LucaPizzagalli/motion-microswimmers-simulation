COMPILER = g++
COMPILER_FLAGS = -std=c++0x -Wall -pedantic -I/home/luca.pizzagalli/swimmers-brownian-simulation/gsl/include -ggdb -O0
LINKER_FLAGS = -std=c++0x -lm -lgsl -lgslcblas -L/home/luca.pizzagalli/swimmers-brownian-simulation/gsl/lib -ggdb
SOURCES = src/*.cpp
OBJECTS := ${subst src/,,$(SOURCES:.cpp=.o)}

all: bin/simulation

bin/simulation: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(LINKER_FLAGS) -o bin/simulation

$(OBJECTS): $(SOURCES) $(SOURCES:.cpp=hpp)
	$(COMPILER) -c $(SOURCES) $(COMPILER_FLAGS)
