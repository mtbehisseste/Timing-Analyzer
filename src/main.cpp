#include <iostream>
#include <unistd.h>

#include "circuit.h"
#include "readFile.h"
#include "utils.h"

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
    // for (auto i = circuit.inputNet.begin(); i != circuit.inputNet.end(); ++i) {
    //     std::cout << i->first  << ' ' << i->second->name << std::endl;
    // }

    std::vector<std::vector<int> > pattern;
    pattern = readPattern(circuit, patternName);

    // for (unsigned int i = 0; i< pattern.size(); ++i) {
    //     for (unsigned int j = 0; j < pattern[0].size();++j) {
    //         cout << pattern[i][j] << ' ' ;
    //     }
    //     cout <<endl;
    // }

    return 0;
}
