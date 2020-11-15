#include <iostream>
#include <iomanip>
#include <queue>

#include "utils.h"

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
    
    while (q.size() > 0) {
        Net *tmp = q.front();
        // cout << "top " << tmp->name << ' ' ;
        // cout << tmp->signal << endl;
        int sig;
        string outputNetName;
        bool noSig;
        for (auto &netConnGate: tmp->inputGate) {
            if (netConnGate.second->visited)
                continue;

            Net *outN = netConnGate.second->outputNet.begin()->second;
            sig = tmp->signal;
            noSig = false;
            for (auto &gateInputNet: netConnGate.second->inputNet) {
                if (gateInputNet.second->signal == -1) {  // signal not generated
                    // cout << "nosig " << gateInputNet.first <<' ' << gateInputNet.second->signal<< endl;
                    noSig = true;
                    q.push(gateInputNet.second);
                    break;
                }

                // calculate output signal
                if (netConnGate.second->footprint == "NANDX1") {
                    // cout << gateInputNet.first << " ";
                    // cout << sig << ' ' << gateInputNet.second->signal << endl;
                    sig &= gateInputNet.second->signal;
                    // cout << sig << endl;
                } else if (netConnGate.second->footprint == "NOR2X1") {
                    // cout << gateInputNet.first << " ";
                    // cout << sig << ' ' << gateInputNet.second->signal << endl;
                    sig |= gateInputNet.second->signal;
                    // cout << sig << endl;
                }
            }
            if (!noSig) {
                outN->signal = sig ^ 1;  // perform not
                // cout << "sig " << outN->name << ' ' << outN->signal << endl;
                q.push(outN);
                netConnGate.second->visited = true;
            }
        }
        q.pop();
    }
    cout << endl;

    for (auto &g: circuit.circuitGate) {
        cout << g.first<< " ";
        cout << g.second->outputNet.begin()->second->signal << endl;
    }
    cout << endl;
}

void reset(Circuit &circuit)
{
    for (auto &n: circuit.allNet)
        n.second->signal = -1;
    for (auto &g: circuit.circuitGate)
        g.second->visited = false;
}
