#include <iostream>
#include <iomanip>
#include <queue>

#include "circuit.h"

using namespace std;

void usage()
{
    cout << "Usage: ./timing_analyzer [path_to_netlist_file] [-p path_to_pattern_file] [-l path_to_cell_library]\n"
        << setw(23) << "path_to_netlist_file: " << "path to netlist file, e.g. ../benchmarks/c17.v\n"
        << setw(23) << "-p: " << "path to pattern file, e.g. -p ../patterns/c17.pat\n"
        << setw(23) << "-l: " << "path to cell library, e.g. -l ../lib/test_lib.lib\n";
    exit(-1);
}

void runPattern(Circuit &circuit, vector<int> pattern)
{
    queue<Net *> q;

    // assign input patter to circuit input
    int patternIdx = 0;
    for (auto &iN: circuit.inputNet) {
        iN.second->signal = pattern[patternIdx++];
        q.push(iN.second);
    }
    
    while (q.front()) {
        Net *tmp = q.front();
        cout << tmp->name << endl;
        string outputNetName;
        for (auto &netConnGate: tmp->inputGate) {
            if (netConnGate.second->visited)
                continue;

            Net *outN = netConnGate.second->outputNet.begin()->second;
            bool noSig = false;
            for (auto &gateInputNet: netConnGate.second->inputNet) {
                if (gateInputNet.second->signal == -1) {  // signal not generated
                    noSig = true;
                    q.push(gateInputNet.second);
                    break;
                }

                // calculate output signal
                if (netConnGate.second->footprint == "NANDX1") {
                    tmp->signal &= gateInputNet.second->signal;
                } else if (netConnGate.second->footprint == "NOR2X1") {
                    tmp->signal |= gateInputNet.second->signal;
                }
            }
            if (!noSig) {
                outN->signal = tmp->signal ^ 1;  // perform not
                q.push(outN);
                netConnGate.second->visited = true;
            }
        }
        q.pop();
    }
    cout << endl;
}
