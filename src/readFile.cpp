#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <algorithm>

#include "readFile.h"

using namespace std;

#define PARSE_TABLE(table)                                                              \
    getline(fLib, strline);                                                             \
    assert(strline.find(#table) != string::npos);                                       \
    for (int i = 0; i < 7; ++i) {                                                       \
        getline(fLib, strline);                                                         \
        first_quote = strline.find_first_of("\"");                                      \
        last_quote = strline.find_last_of("\"");                                        \
        sf = strline.substr(first_quote + 1, last_quote - first_quote - 1);             \
        for (int j = 0; j < 7; ++j) {                                                   \
            sscanf(sf.c_str(), "%f%*s", &f);                                            \
            sf = sf.substr(sf.find_first_of(",") + 1);                                  \
            cell->table.insert(make_pair(                                               \
                        make_pair(outputCapacitance[i], inputTransitionTime[j]), f));   \
        }                                                                               \
    }                                                                                   \
    getline(fLib, strline);

#define FIND_NET(circuitNet)                                                        \
    while (true) {                                                                  \
        subline = line;                                                             \
        subline.erase(remove(subline.begin(), subline.end(), ' '), subline.end());  \
        while (subline.find(",") != string::npos) {                                 \
            sscanf(subline.c_str(), "%[^,]", netName);                              \
            Net *net = new Net(string(netName));                                    \
            circuit.circuitNet.insert(make_pair(net->name, net));             \
            subline = subline.substr(subline.find_first_of(",") + 1);               \
        }                                                                           \
                                                                                    \
        if (subline.find(";") != string::npos) {                                    \
            sscanf(subline.c_str(), "%[^;]", netName);                              \
            Net *net = new Net(string(netName));                                            \
            circuit.circuitNet.insert(make_pair(net->name, net));             \
            break;                                                                  \
        }                                                                           \
                                                                                    \
        getline(fsCircuit, line);                                                   \
    }

void readLibrary(map<string, Gate *> &libCell, string libName)
{
    ifstream fLib;
    fLib.open(libName.c_str(), ios::in);
    if (!fLib) {
        cout << "Error opening library file\n";
        exit(-1);
    }

    string strline;
    size_t first_quote, last_quote;

    // skip unit attributes
    for (int i = 0; i < 9; ++i)
        getline(fLib, strline);

    // record output net capacitances and input transition time
    float outputCapacitance[7], inputTransitionTime[7];

    getline(fLib, strline);  // index 1
    sscanf(strline.c_str(), "%*s \(\"%f,%f,%f,%f,%f,%f,%f\"%*s", &outputCapacitance[0],
            &outputCapacitance[1], &outputCapacitance[2], &outputCapacitance[3],
            &outputCapacitance[4], &outputCapacitance[5], &outputCapacitance[6]);

    getline(fLib, strline);  // index 2
    sscanf(strline.c_str(), "%*s \(\"%f,%f,%f,%f,%f,%f,%f\"%*s", &inputTransitionTime[0],
            &inputTransitionTime[1], &inputTransitionTime[2], &inputTransitionTime[3],
            &inputTransitionTime[4], &inputTransitionTime[5], &inputTransitionTime[6]);

    // parse library cells
    char s[20];
    float f;
    while (!fLib.eof()) {
        // cell
        while (getline(fLib, strline)) {
            if (strline.find("cell") != string::npos)
                break;
        }
        sscanf(strline.c_str(), "cell (%[^)] {", s);
        Gate *cell = new Gate(string(s));

        // pin
        while (getline(fLib, strline)) {
            assert(strline.find("pin") != string::npos);
            sscanf(strline.c_str(), " pin(%[^)]", s);
            Pin *pin = new Pin(string(s));

            getline(fLib, strline);
            sscanf(strline.c_str(), " direction : %[^;]", s);
            pin->direction = (string(s) == "input" ? 0 : 1);

            getline(fLib, strline);
            sscanf(strline.c_str(), " capacitance : %f;", &f);
            pin->capacitance = f;

            // if is output pin
            if (pin->direction == 1) {
                string sf;

                getline(fLib, strline);  // internal_power()
                PARSE_TABLE(rise_power);
                PARSE_TABLE(fall_power);
                getline(fLib, strline);  // }

                getline(fLib, strline);  // timing() 
                PARSE_TABLE(cell_rise);
                PARSE_TABLE(cell_fall);
                PARSE_TABLE(rise_transition);
                PARSE_TABLE(fall_transition);
                getline(fLib, strline);  // }
                
                getline(fLib, strline);  // }
                getline(fLib, strline);  // }
                getline(fLib, strline);  // }
                cell->outputPin.insert(make_pair(pin->footprint, pin));
                break;  // output pin must be the last pin
            } else {
                getline(fLib, strline);  // }
                cell->inputPin.insert(make_pair(pin->footprint, pin));
            }
        }

        libCell.insert(make_pair(cell->name, cell));
    }
}

void readCircuit(Circuit &circuit, string circuitName,
        map<string, Gate *> libCell)
{
    ifstream fsCircuit;
    fsCircuit.open(circuitName, ios::in);
    if (!fsCircuit) {
        cout << "Error opening circuit file\n";
        exit(-1);
    }

    string line, subline;
    char netName[20];

    // TODO deal with comments: process once a line
    // parsing inputs, outputs, wires
    int netFlag = 0;
    while (getline(fsCircuit, line)) {
        if (line.find("input") != string::npos) {
            if (line.find("//") == string::npos ||
                    (line.find("//") != string::npos &&
                    line.find("input") < line.find("//"))) {
                line = line.substr(line.find("input") + 6);
                FIND_NET(inputNet);
                netFlag++;
            }
        } else if (line.find("output") != string::npos) {
            if (line.find("//") == string::npos ||
                    (line.find("//") != string::npos &&
                    line.find("output") < line.find("//"))) {
                line = line.substr(line.find("output") + 7);
                FIND_NET(outputNet);
                netFlag++;
            }
        } else if (line.find("wire") != string::npos) {
            if (line.find("//") == string::npos ||
                    (line.find("//") != string::npos &&
                    line.find("wire") < line.find("//"))) {
                line = line.substr(line.find("wire") + 5);
                FIND_NET(wireNet);
                netFlag++;
            }
        }
        if (netFlag == 3)
            break;  // input, output, wire are parsed
    }

    // parsing cells
    char cellFootprint[20];
    char cellName[20];
    int commentFlag = 0;
    while (getline(fsCircuit, line)) {
        // deal with different comments
        if (commentFlag == 1) {
            if (line.find("*/") != string::npos) {
                line = line.substr(line.find("*/") + 2);
                commentFlag = 0;
            } else
                continue;
        }
        if (line.find("//") != string::npos)
            line = line.substr(0, line.find("//") - 1);
        if (line.find("/*") != string::npos) {
            if (line.find("*/") != string::npos) {
                line = line.substr(0, line.find("/*") - 1)
                    + line.substr(line.find("*/") + 2);
            } else {
                line = line.substr(0, line.find("/*") - 1);
                commentFlag = 1;
            }
        }

        if (line.find(";") == string::npos)
            continue;
        if (line.find("endmodule") != string::npos)
            break;

        replace(line.begin(), line.end(), '(', ' ');
        replace(line.begin(), line.end(), ')', ' ');
        replace(line.begin(), line.end(), '.', ' ');
        replace(line.begin(), line.end(), ',', ' ');

        sscanf(line.c_str(), "%s %s", cellFootprint, cellName);
        line = line.substr(line.find(string(cellName)) + string(cellName).length(),
                line.length() - line.find(string(cellName)));
        Gate *gate = new Gate(string(cellFootprint));
        gate->name = string(cellName);
        
        char type1[10], type2[10], type3[10], value1[10], value2[10], value3[10];
        string input1, input2, output;
        if (string(cellFootprint) == "INVX1") {
            sscanf(line.c_str(), "%s %s %s %s", type1, value1, type2, value2);
            if (string(type1) == "ZN") {
                output = string(value1);
                input1 = string(value2);
            } else if (string(type1).find("I") != string::npos) {
                input1 = string(value1);
                output = string(value2);
            }

            Net *i1 = new Net(string(input1));
            Net *o = new Net(string(output));
            i1->inputGate.insert(make_pair(gate->name, gate));
            o->outputGate.insert(make_pair(gate->name, gate));
            gate->inputNet.insert(make_pair(string(input1), i1));
            gate->outputNet.insert(make_pair(string(output), o));
            cout << input1 << ' ' << output  << endl;

        } else {
            sscanf(line.c_str(), "%s %s %s %s %s %s", type1, value1,
                    type2, value2, type3, value3);
            if (string(type1) == "ZN") {
                output = string(value1);
                input1 = string(value2);
                input2 = string(value3);
            } else if (string(type1).find("A") != string::npos) {
                input1 = string(value1);
                input2 = string(value2);
                output = string(value3);
            }

            Net *i1 = new Net(string(input1));
            Net *i2 = new Net(string(input2));
            Net *o = new Net(string(output));
            i1->inputGate.insert(make_pair(gate->name, gate));
            i2->inputGate.insert(make_pair(gate->name, gate));
            o->outputGate.insert(make_pair(gate->name, gate));
            gate->inputNet.insert(make_pair(string(input1), i1));
            gate->inputNet.insert(make_pair(string(input1), i2));
            gate->outputNet.insert(make_pair(string(output), o));
            cout << input1 << ' ' << input2 << ' ' << output  << endl;
        }

        circuit.circuitGate.insert(make_pair(gate->name, gate));
    }
    
}
