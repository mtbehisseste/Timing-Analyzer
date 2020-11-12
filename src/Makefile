EXE = timing_analyzer
OBJS = main.o readFile.o

all: $(EXE)

$(EXE): $(OBJS)
	g++ -g -o $@ $^

%.o: %.cpp
	g++ -c -o $@ $^
