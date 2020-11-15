#include <vector>
#include <map>
#include "circuit.h"

void usage();
void runPattern(Circuit &circuit, std::vector<int> pattern,
        std::map<std::string, Gate *> libCell);
void reset(Circuit &circuit);
