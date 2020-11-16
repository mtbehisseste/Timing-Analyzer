#include <iostream>
#include <unistd.h>

#include "circuit.h"
#include "readFile.h"
#include "utils.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 6) {
        cout << "Error number of command line arguments\n";
        usage();
    }

    // parse command line arguments 
    string circuitName = string(argv[1]);
    string libName;
    string patternName;
    int cmdOpt = 0;

    while ((cmdOpt = getopt(argc, argv, "p:l:")) != -1) {
        switch (cmdOpt) {
        case 'p':
            patternName = string(optarg);
            break;
        case 'l':
            libName = string(optarg);
            break;
        case '?':
        default:
            usage();
        }
    }
 
    map<string, Cell *> libCell;
    readLibrary(libCell, libName);

    Circuit circuit;
    readCircuit(circuit, circuitName);
    // for (auto i = circuit.inputNet.begin(); i != circuit.inputNet.end(); ++i) {
    //     cout << i->first  << ' ' << i->second->name << endl;
    // }

    vector<vector<int> > pattern;
    pattern = readPattern(circuit, patternName);
    // for (unsigned int i = 0; i< pattern.size(); ++i) {
    //     for (unsigned int j = 0; j < pattern[0].size();++j) {
    //         cout << pattern[i][j] << ' ' ;
    //     }
    //     cout <<endl;
    // }

    // run each pattern
    for (unsigned int i = 0; i < pattern.size(); ++i) {
        runPattern(circuit, pattern[i], libCell);
        reset(circuit);
    }


    // TODO delet all newed objects

    return 0;
}
