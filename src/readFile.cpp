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
            cell->table.insert({                                                        \
                    make_pair(outputCapacitance[i], inputTransitionTime[j]), f});       \
        }                                                                               \
    }                                                                                   \
    getline(fLib, strline);

#define FIND_NET(circuitNetVector, typeNum)                                         \
    while (true) {                                                                  \
        subline = line;                                                             \
        subline.erase(remove(subline.begin(), subline.end(), ' '), subline.end());  \
        while (subline.find(",") != string::npos) {                                 \
            sscanf(subline.c_str(), "%[^,]", netName);                              \
            Net *net = new Net(string(netName));                                    \
            net->type = typeNum;                                                    \
            circuit.circuitNetVector.push_back(net->name);                          \
            circuit.allNet.insert({net->name, net});                                \
            subline = subline.substr(subline.find_first_of(",") + 1);               \
        }                                                                           \
                                                                                    \
        if (subline.find(";") != string::npos) {                                    \
            sscanf(subline.c_str(), "%[^;]", netName);                              \
            Net *net = new Net(string(netName));                                    \
            net->type = typeNum;                                                    \
            circuit.circuitNetVector.push_back(net->name);                          \
            circuit.allNet.insert({net->name, net});                                \
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
                cell->outputPin.insert({pin->footprint, pin});
                break;  // output pin must be the last pin
            } else {
                getline(fLib, strline);  // }
                cell->inputPin.insert({pin->footprint, pin});
            }
        }

        libCell.insert({cell->footprint, cell});
    }
}

void readCircuit(Circuit &circuit, string circuitName,
        map<string, Gate *> libCell)
{
    ifstream fsCircuit;
    fsCircuit.open(circuitName.c_str(), ios::in);
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
                FIND_NET(inputNetName, 0);
                netFlag++;
            }
        } else if (line.find("output") != string::npos) {
            if (line.find("//") == string::npos ||
                    (line.find("//") != string::npos &&
                    line.find("output") < line.find("//"))) {
                line = line.substr(line.find("output") + 7);
                FIND_NET(outputNetName, 1);
                netFlag++;
            }
        } else if (line.find("wire") != string::npos) {
            if (line.find("//") == string::npos ||
                    (line.find("//") != string::npos &&
                    line.find("wire") < line.find("//"))) {
                line = line.substr(line.find("wire") + 5);
                FIND_NET(wireNetName, 2);
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
        Pin *i1, *i2, *o;
        if (string(cellFootprint) == "INVX1") {
            sscanf(line.c_str(), "%s %s %s %s", type1, value1, type2, value2);
            if (string(type1) == "ZN") {
                i1 = new Pin(string(type2));
                o = new Pin(string(type1));
                output = string(value1);
                input1 = string(value2);
            } else if (string(type1).find("I") != string::npos) {
                i1 = new Pin(string(type1));
                o = new Pin(string(type2));
                input1 = string(value1);
                output = string(value2);
            }
            gate->inputPin.insert({input1, i1});
            gate->outputPin.insert({output, o});

            circuit.allNet[input1]->inputGateName.push_back(gate->name);
            circuit.allNet[output]->outputGateName.push_back(gate->name);
            gate->inputNetName.push_back(input1);
            gate->outputNetName.push_back(output);

        } else {
            sscanf(line.c_str(), "%s %s %s %s %s %s", type1, value1,
                    type2, value2, type3, value3);
            if (string(type1) == "ZN") {
                i1 = new Pin(string(type2));
                i2 = new Pin(string(type3));
                o = new Pin(string(type1));
                output = string(value1);
                input1 = string(value2);
                input2 = string(value3);
            } else if (string(type1).find("A") != string::npos) {
                i1 = new Pin(string(type1));
                i2 = new Pin(string(type2));
                o = new Pin(string(type3));
                input1 = string(value1);
                input2 = string(value2);
                output = string(value3);
            }
            gate->inputPin.insert({input1, i1});
            gate->inputPin.insert({input2, i2});
            gate->outputPin.insert({output, o});

            circuit.allNet[input1]->inputGateName.push_back(gate->name);
            circuit.allNet[input2]->inputGateName.push_back(gate->name);
            circuit.allNet[output]->outputGateName.push_back(gate->name);
            gate->inputNetName.push_back(input1);
            gate->inputNetName.push_back(input2);
            gate->outputNetName.push_back(output);
        }

        circuit.circuitGateName.push_back(gate->name);
        circuit.circuitGate.insert({gate->name, gate});
    }
}

vector<vector<int> > readPattern(Circuit &circuit, string patternName)
{
    ifstream fsPattern;
    fsPattern.open(patternName.c_str(), ios::in);
    if (!fsPattern) {
        cout << "Error opening pattern file\n";
        exit(-1);
    }

    int inputNetNum = circuit.inputNetName.size();
    string line;
    char num[1];
    vector<vector<int> > pattern;
    while (getline(fsPattern, line)) {
        if (line.find("end") != string::npos)
            return pattern;
        if (line.find("input") != string::npos)
            continue;
        if (line.find("//") == 0)
            continue;

        vector<int> tmp;
        for (int i = 0; i < inputNetNum; ++i) {
            sscanf(line.c_str(), "%s", num);
            tmp.push_back(atoi(num));
            line = line.substr(line.find(num) + 1);
        }
        pattern.push_back(tmp);
    }
    return pattern;
}
