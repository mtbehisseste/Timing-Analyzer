#include <vector>
#include <map>
#include <fstream>
#include "circuit.h"

void usage();
void runPattern(Circuit &circuit, std::vector<int> pattern,
        std::map<std::string, Cell *> libCell);
void reset(Circuit &circuit);
void outputToFile(Circuit &circuit, std::ofstream &fout);
