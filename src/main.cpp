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

    vector<vector<int> > pattern;
    pattern = readPattern(circuit, patternName);

    // output stream
    circuitName = circuitName.substr(circuitName.find_last_of("/") + 1);
    string outputFileName = "0866015_" +
        circuitName.substr(0, circuitName.find(".v")) + ".txt";

    ofstream fout;
    fout.open(outputFileName.c_str(), ios::out);
    if (!fout) {
        cout << "Error opening output file\n";
        exit(-1);
    }

    // run each pattern
    for (unsigned int i = 0; i < pattern.size(); ++i) {
        runPattern(circuit, pattern[i], libCell);
        outputToFile(circuit, fout);
        reset(circuit);
    }

    // TODO delet all newed objects

    return 0;
}
