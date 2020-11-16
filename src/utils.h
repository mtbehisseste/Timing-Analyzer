#include <vector>
#include <map>
#include "circuit.h"

void usage();
void runPattern(Circuit &circuit, std::vector<int> pattern,
        std::map<std::string, Cell *> libCell);
void reset(Circuit &circuit);
