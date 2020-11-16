#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>
#include <map>
#include <stack>

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
    //cout << "=======================" << endl;
    //cout << "o1 o2 " << o1 << "  " << o2 << endl;
    //cout << "i1 i2 " << i1 << "  " << i2 << endl;
    float t1, t2, t3, t4;  // table order left->right, up->down
    float tmp1, tmp2, result;
    t1 = table[make_pair(i1, o1)];
    t2 = table[make_pair(i1, o2)];
    t3 = table[make_pair(i2, o1)];
    t4 = table[make_pair(i2, o2)];
    //cout << "t1 t2 " << t1 << "  " << t2 << endl;
    //cout << "t3 t4 " << t3 << "  " << t4 << endl;
    tmp1 = t1 + (inputTransitionTime - i1) / (i2 - i1) * (t3 - t1);
    tmp2 = t2 + (inputTransitionTime - i1) / (i2 - i1) * (t4 - t2);
    //cout << "tmp1 tmp2 " << tmp1 << "  " << tmp2 << endl;
    result = tmp1 + (outputCapacitance - o1) / (o2 - o1) * (tmp2 - tmp1);
    //cout << "result " << result << endl;
    //cout << "+++++++++++++++++++++++" << endl;
    return result;
}

void calcDelay(Circuit &circuit, map<string, Cell *> libCell, Gate *&gate, int outputSignal)
{
    float outputCapacitance = 0.0;
    float inputTransitionTime = 0.0;

    //cout << "Current gate " << gate->name << endl;

    // find pins of the successor gates to calculate output capacitance
    Pin *p;
    string cellFootprint;
    for (auto netConnGateName: circuit.allNet[gate->outputNetName[0]]->inputGateName) {
        //cout << "successing gate: " <<netConnGateName<< endl;
        cellFootprint = circuit.circuitGate[netConnGateName]->footprint;
        p = circuit.circuitGate[netConnGateName]->inputPin[gate->outputNetName[0]];
        outputCapacitance += libCell[cellFootprint]->inputPin[p->footprint]->capacitance;
        //cout << "   input capacitance:" << libCell[cellFootprint]->inputPin[p->footprint]->capacitance << endl;
    }
    if (outputCapacitance == 0)
        outputCapacitance = 0.03;

    //cout << "output capacitance: " << outputCapacitance << endl;

    // find preceding gates of all the input nets of the gate, then get their output
    // capacitance
    float maxInputTransitionTime = 0.0;
    for (auto inNetName: gate->inputNetName) {
        //cout << "input net: " << inNetName << endl;
        for (auto outGateName: circuit.allNet[inNetName]->outputGateName) {
            //cout << "   preceding gate: " << outGateName << endl;
            maxInputTransitionTime = max(maxInputTransitionTime, 
                    circuit.circuitGate[outGateName]->outputTransition);
            //cout << "       Max: " << maxInputTransitionTime << endl;
        }
    }
    inputTransitionTime = maxInputTransitionTime;
    //cout << "input transition time: " << inputTransitionTime<< endl;

    // finding table index
    float o1 = 0.0, o2 = 0.0, i1 = 0.0, i2 = 0.0;
    for (auto it = libCell[gate->footprint]->cell_fall.begin();
            it != libCell[gate->footprint]->cell_fall.end(); ++it) {
        if (it->first.second > outputCapacitance) {
            o1 = prev(it, 1)->first.second;
            o2 = it->first.second;
            break;
        }
    }

    if (inputTransitionTime < libCell[gate->footprint]->index2[0]) {
        // perform extrapolation
        i1 = libCell[gate->footprint]->index2[0];
        i2 = libCell[gate->footprint]->index2[1];
        //cout << "i " << i1 << ' ' << i2 << endl;
    } else {
        for (auto it = libCell[gate->footprint]->cell_fall.begin();
                it != libCell[gate->footprint]->cell_fall.end(); ++it) {
            if (it->first.first > inputTransitionTime) {
                i1 = prev(it, 1)->first.first;
                i2 = it->first.first;
                break;
            }
        }
    }

    float cellFall = 0.0, cellRise = 0.0;
    if (outputSignal == 0) {
        gate->outputTransition = interpolation(libCell[gate->footprint]->fall_transition,
                outputCapacitance, inputTransitionTime, o1, o2, i1, i2);
        cellFall = interpolation(libCell[gate->footprint]->cell_fall,
                outputCapacitance, inputTransitionTime, o1, o2, i1, i2);
    } else if (outputSignal == 1) {
        gate->outputTransition = interpolation(libCell[gate->footprint]->rise_transition,
                outputCapacitance, inputTransitionTime, o1, o2, i1, i2);
        cellRise = interpolation(libCell[gate->footprint]->cell_rise,
                outputCapacitance, inputTransitionTime, o1, o2, i1, i2);
    }
    gate->cellDelay = max(cellFall, cellRise);

    // find longest delay
    float longestPrecedingDelay = 0.0;
    string longestDelayGateName = "PI";
    for (auto inNetName: gate->inputNetName) {
        for (auto outGateName: circuit.allNet[inNetName]->outputGateName) {
            if (circuit.circuitGate[outGateName]->currentMaxDelay >= longestPrecedingDelay) {
                longestPrecedingDelay = circuit.circuitGate[outGateName]->currentMaxDelay;
                longestDelayGateName = outGateName;
                // cout << longestDelayNetName << endl;
            }
        }
    }
    gate->maxDelayPrecedingGateName = longestDelayGateName;
    gate->currentMaxDelay = gate->cellDelay + longestPrecedingDelay;
}

void runPattern(Circuit &circuit, vector<int> pattern, map<string, Cell *> libCell)
{
    queue<Net *> q;

    // assign input patter to circuit input
    int patternIdx = 0;
    for (auto &iN: circuit.inputNetName) {
        circuit.allNet[iN]->signal = pattern[patternIdx++];
        q.push(circuit.allNet[iN]);
    }
    
    float maxDelay = -1;
    string maxDelayGateName;
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

            // find all input signal of the gate and calculate output signal
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

        // if reach output nets, record the maximum delay and last cell
        if (tmp->type == 1) {
            for (auto netConnGateName: tmp->outputGateName) {
                 if (circuit.circuitGate[netConnGateName]->currentMaxDelay > maxDelay) {
                    maxDelay = circuit.circuitGate[netConnGateName]->currentMaxDelay;
                    maxDelayGateName = netConnGateName;
                 }
            }
        }
    }

    circuit.maxDelay = maxDelay;
    Gate *g = circuit.circuitGate[maxDelayGateName];
    while (true) {
        circuit.longestPath.push(g->outputNetName[0]);
        if (g->maxDelayPrecedingGateName == "PI") {
            // reach input nets
            circuit.longestPath.push(g->inputNetName[0]);
            break;
        }
        g = circuit.circuitGate[g->maxDelayPrecedingGateName];
    }
}

void reset(Circuit &circuit)
{
    for (auto &n: circuit.allNet)
        n.second->signal = -1;
    for (auto &g: circuit.circuitGate)
        g.second->visited = false;
}

void outputToFile(Circuit &circuit, ofstream &fout)
{
    fout << "Longest delay = " << circuit.maxDelay << ", the path is:\n";
    while (circuit.longestPath.size() > 0) {
        fout << circuit.longestPath.top();
        if (circuit.longestPath.size() > 1)
            fout << " -> ";
        circuit.longestPath.pop();
    }
    fout << endl << endl;

    // output
    for (auto &g: circuit.circuitGateName) {
        fout << g << " ";
        fout << circuit.allNet[circuit.circuitGate[g]->outputNetName[0]]->signal << " ";
        fout << circuit.circuitGate[g]->cellDelay;
        fout << " ";
        fout << circuit.circuitGate[g]->outputTransition << endl;
    }
    fout << endl;
}
