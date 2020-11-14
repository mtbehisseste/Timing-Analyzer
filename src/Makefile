EXE = timing_analyzer
OBJS = main.o readFile.o utils.o
CFLAGS = -std=c++11 -g -Wall

all: $(EXE)

$(EXE): $(OBJS)
	g++ $(CFLAGS) -o $@ $^

%.o: %.cpp
	g++ $(CFLAGS) -c -o $@ $^
