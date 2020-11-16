#include <vector>

#include "circuit.h"

void readLibrary(std::map<std::string, Cell *> &libCell, std::string libName);
void readCircuit(Circuit &circuit, std::string circuitName);
std::vector<std::vector<int> > readPattern(Circuit &circuit, std::string patternName);
