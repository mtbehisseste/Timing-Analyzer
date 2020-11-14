#include <vector>

#include "circuit.h"

void readLibrary(std::map<std::string, Gate *> &libCell, std::string libName);
void readCircuit(Circuit &circuit, std::string circuitName,
        std::map<std::string, Gate *> libCell);
std::vector<std::vector<int> > readPattern(Circuit circuit, std::string patternName);
