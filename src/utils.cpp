#include <iostream>
#include <iomanip>
#include <queue>
#include <map>

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

float interpolation(map<pair<float, float>, float> &table,
        float outputCapacitance, float inputTransitionTime,
        float o1, float o2, float i1, float i2)
{
    float t1, t2, t3, t4;  // table order left->right, up->down
    float tmp1, tmp2;
    t1 = table[make_pair(o1, i1)];
    t2 = table[make_pair(o2, i1)];
    t3 = table[make_pair(o1, i2)];
    t4 = table[make_pair(o2, i2)];
    tmp1 = t1 + (inputTransitionTime - i1) / (i2 - i1) * (t3 - t1);
    tmp2 = t2 + (inputTransitionTime - i1) / (i2 - i1) * (t4 - t2);
    return tmp1 + (outputCapacitance - o1) / (o2 - o1) * (tmp2 - tmp1);
}

void calcDelay(Circuit &circuit, map<string, Gate *> libCell, Gate *&gate, int outputSignal)
{
    float outputCapacitance = 0.0;
    float inputTransitionTime = 0.0;

    // find pins of the successor gates to calculate output capacitance
    Pin *p;
    string cellFootprint;
    for (auto netConnGateName: circuit.allNet[gate->outputNetName[0]]->inputGateName) {
        cellFootprint = circuit.circuitGate[netConnGateName]->footprint;
        p = circuit.circuitGate[netConnGateName]->inputPin[gate->outputNetName[0]];
        outputCapacitance += libCell[cellFootprint]->inputPin[p->footprint]->capacitance;
    }

    // find preceding gates of all the input nets of the gate, then get their output
    // capacitance
    for (auto inNetName: gate->inputNetName) {
        for (auto outGateName: circuit.allNet[inNetName]->outputGateName) {
            inputTransitionTime += circuit.circuitGate[outGateName]->outputTransition;
        }
    }

    if (outputCapacitance == 0)
        outputCapacitance = 0.3;

    // calculate output transition time
    float o1 = 0.0, o2 = 0.0, i1 = 0.0, i2 = 0.0;
    for (auto it = libCell[gate->footprint]->cell_fall.begin();
            it != libCell[gate->footprint]->cell_fall.end(); ++it) {
        if (it->first.first < outputCapacitance)
            o1 = it->first.first;
        else {
            o2 = it->first.first;
            break;
        }
    }
    for (auto it = libCell[gate->footprint]->cell_fall.begin();
            it != libCell[gate->footprint]->cell_fall.end(); ++it) {
        if (inputTransitionTime == 0) {
            i1 = libCell[gate->footprint]->cell_fall.begin()->first.second;
            i2 = next(libCell[gate->footprint]->cell_fall.begin(), 1)->first.second;
            break;
        }
        if (it->first.second < inputTransitionTime)
            i1 = it->first.second;
        else {
            i2 = it->first.second;
            break;
        }
    }

    if (outputSignal == 0) {
        gate->outputTransition = interpolation(libCell[gate->footprint]->fall_transition,
                outputCapacitance, inputTransitionTime, o1, o2, i1, i2) / 2;
    } else if (outputSignal == 1) {
        gate->outputTransition = interpolation(libCell[gate->footprint]->rise_transition,
                outputCapacitance, inputTransitionTime, o1, o2, i1, i2) / 2;
    }
}

void runPattern(Circuit &circuit, vector<int> pattern, map<string, Gate *> libCell)
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
            // if (circuit.circuitGate[netConnGateName]->visited)
            //     continue;

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

                calcDelay(circuit, libCell, circuit.circuitGate[netConnGateName], outN->signal);
            }

        }
        q.pop();
    }
    cout << endl;

    for (auto &g: circuit.circuitGateName) {
        cout << g << " ";
        cout << circuit.allNet[circuit.circuitGate[g]->outputNetName[0]]->signal << " ";
        cout << circuit.circuitGate[g]->outputTransition << endl;
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

