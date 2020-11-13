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
            cout << netName << endl;                                                \
            Net *net = new Net(string(netName));                                    \
            circuit.circuitNet.push_back(net);                                      \
            subline = subline.substr(subline.find_first_of(",") + 1);               \
        }                                                                           \
                                                                                    \
        if (subline.find(";") != string::npos) {                                    \
            sscanf(subline.c_str(), "%[^;]", netName);                              \
            cout << netName << endl;                                                \
            Net *net = new Net(netName);                                            \
            circuit.circuitNet.push_back(net);                                      \
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
                size_t first_quote, last_quote;
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
                cell->output.push_back(pin);
                break;  // output pin must be the last pin
            } else {
                getline(fLib, strline);  // }
                cell->input.push_back(pin);
            }
        }

        cout << endl;
        for (int i = 0; i < 7; ++i) {
            for (int j = 0; j < 7; ++j) {
                cout << cell->cell_fall[pair<float, float>(outputCapacitance[i], inputTransitionTime[j])] << ' ';
            }
            cout << endl;
        }

        cout << cell->input.size() << endl;
        for (int i = 0; i < cell->input.size(); ++i) {
            cout << cell->input[i]->capacitance<< endl;
        }

        libCell.insert(make_pair(cell->name, cell));
    }
}

void readCircuit(Circuit &circuit, string circuitName)
{
    ifstream fsCircuit;
    fsCircuit.open(circuitName, ios::in);
    if (!fsCircuit) {
        cout << "Error opening circuit file\n";
        exit(-1);
    }

    string line, subline;
    char netName[20];

    while (getline(fsCircuit, line)) {
        if (line.find("input") != string::npos) {
            if (line.find("//") == string::npos ||
                    (line.find("//") != string::npos &&
                    line.find("input") < line.find("//"))) {
                line = line.substr(line.find("input") + 6);
                FIND_NET(inputNet);
            }
        } else if (line.find("output") != string::npos) {
            if (line.find("//") == string::npos ||
                    (line.find("//") != string::npos &&
                    line.find("output") < line.find("//"))) {
                line = line.substr(line.find("output") + 7);
                FIND_NET(outputNet);
            }
        } else if (line.find("wire") != string::npos) {
            if (line.find("//") == string::npos ||
                    (line.find("//") != string::npos &&
                    line.find("wire") < line.find("//"))) {
                line = line.substr(line.find("wire") + 5);
                FIND_NET(wireNet);
            }
        }
    }
    
}
