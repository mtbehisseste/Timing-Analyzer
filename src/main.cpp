#include "circuit.h"
#include "readFile.h"

int main() {
    // TODO argparse
    
    std::string libName = "../lib/test_lib.lib";
    std::string circuitName = "../benchmarks/c432.v";
    
    std::map<std::string, Gate *> libCell;
    readLibrary(libCell, libName);

    Circuit circuit;
    readCircuit(circuit, circuitName);

    return 0;
}
