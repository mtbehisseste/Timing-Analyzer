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
    for (auto &iN: circuit.inputNetName) {
        circuit.allNet[iN]->signal = pattern[patternIdx++];
        q.push(circuit.allNet[iN]);
    }
    
    while (q.size() > 0) {
        Net *tmp = q.front();
        // cout << "top " << tmp->name << ' ' ;
        // cout << tmp->signal << endl;
        if (tmp->signal == -1) {
            q.pop();
            q.push(tmp);
            continue;
        }

        int sig;
        bool noSig;
        for (auto &netConnGateName: tmp->inputGateName) {
            if (circuit.circuitGate[netConnGateName]->visited)
                continue;

            Net *outN = circuit.allNet[circuit.circuitGate[netConnGateName]->outputNetName[0]];
            sig = tmp->signal;
            noSig = false;
            for (auto &gateInputNetName: circuit.circuitGate[netConnGateName]->inputNetName) {
                if (circuit.allNet[gateInputNetName]->signal == -1) {  // signal not generated
                    // cout << "nosig " << gateInputNet.first <<' ' << circuit.allNet[gateInputNetName]->signal<< endl;
                    noSig = true;
                    q.push(circuit.allNet[gateInputNetName]);
                    break;
                }

                // calculate output signal
                if (circuit.circuitGate[netConnGateName]->footprint == "NANDX1") {
                    // cout << gateInputNet.first << " ";
                    // cout << sig << ' ' << circuit.allNet[gateInputNetName]->signal << endl;
                    sig &= circuit.allNet[gateInputNetName]->signal;
                    // cout << sig << endl;
                } else if (circuit.circuitGate[netConnGateName]->footprint == "NOR2X1") {
                    // cout << gateInputNet.first << " ";
                    // cout << sig << ' ' << circuit.allNet[gateInputNetName]->signal << endl;
                    sig |= circuit.allNet[gateInputNetName]->signal;
                    // cout << sig << endl;
                }
            }
            if (!noSig) {
                outN->signal = sig ^ 1;  // perform not
                // cout << "sig " << outN->name << ' ' << outN->signal << endl;
                q.push(outN);
                circuit.circuitGate[netConnGateName]->visited = true;
            }
        }
        q.pop();
    }
    cout << endl;

    for (auto &g: circuit.circuitGateName) {
        cout << g<< " ";
        cout << circuit.allNet[circuit.circuitGate[g]->outputNetName[0]]->signal << endl;
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
