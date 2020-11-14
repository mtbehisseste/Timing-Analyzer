#include <iostream>
#include <unistd.h>

#include "circuit.h"
#include "readFile.h"

void usage() {
    std::cout << "Usage: ./timing_analyzer [path_to_netlist_file] [-p path_to_pattern_file] [-l path_to_cell_library]\n"
        << "\tpath_to_netlist_file: path to netlist file, e.g. ../benchmarks/c17.v\n"
        << "\t-p: path to pattern file, e.g. -p ../patterns/c17.pat\n"
        << "\t-l: path to cell librayr, e.g. -l ../lib/test_lib.lib\n";
    exit(-1);
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        std::cout << "Error number of command line arguments\n";
        usage();
    }

    // parse command line arguments 
    std::string circuitName = std::string(argv[1]);
    std::string libName;
    std::string patternName;
    int cmdOpt = 0;

    while ((cmdOpt = getopt(argc, argv, "p:l:")) != -1) {
        switch (cmdOpt) {
        case 'p':
            patternName = std::string(optarg);
            break;
        case 'l':
            libName = std::string(optarg);
            break;
        case '?':
        default:
            usage();
        }
    }
    
    std::map<std::string, Gate *> libCell;
    readLibrary(libCell, libName);

    Circuit circuit;
    readCircuit(circuit, circuitName, libCell);
    // for (int i = 0; i< circuit.inputNet.size(); ++i) {
    //     std::cout << circuit.wireNet[i]->name << std::endl;
    // }

    return 0;
}
