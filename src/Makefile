EXE = timing_analyzer
OBJS = main.o readFile.o utils.o
CFLAGS = -std=c++11 -g -Wall
BENCHMARKS = c17 c432 example

all: $(EXE)

$(EXE): $(OBJS)
	g++ $(CFLAGS) -o $@ $^

%.o: %.cpp
	g++ $(CFLAGS) -c -o $@ $^

$(BENCHMARKS): $(EXE)
	./$< ../benchmarks/$@.v -p ../patterns/$@.pat -l ../lib/test_lib.lib
