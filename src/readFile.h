#include "circuit.h"

void readLibrary(std::map<std::string, Gate *> &libCell, std::string libName);
void readCircuit(Circuit &circuitHead, std::string circuitName,
        std::map<std::string, Gate *> libCell);
