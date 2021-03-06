#ifndef _CIRCUIT_H_
#define _CIRCUIT_H_

#include <string>
#include <vector>
#include <map>
#include <stack>

class Net;

/*
 * data structure of pin
 */
class Pin {
public:
    std::string footprint;
    int direction = -1;  // 0 for input, 1 for output, -1 for uninitialized
    double capacitance = 0.0;

    Pin(std::string f): footprint(f) {}
private:
};

/*
 * data structure of library cell
 */
class Cell {
public:
    std::string footprint;
    std::map<std::string, Pin *> inputPin;
    std::map<std::string, Pin *> outputPin;
    std::vector<double> index1;
    std::vector<double> index2;
    std::map<std::pair<double, double>, double> rise_power;
    std::map<std::pair<double, double>, double> fall_power;
    std::map<std::pair<double, double>, double> cell_rise;
    std::map<std::pair<double, double>, double> cell_fall;
    std::map<std::pair<double, double>, double> rise_transition;
    std::map<std::pair<double, double>, double> fall_transition;

    Cell(std::string footprint): footprint(footprint) {}

    void deleteClass() {
        for (auto &ipin: inputPin) {
            delete ipin.second;
        }
        for (auto &opin: outputPin) {
            delete opin.second;
        }
    }

private:
};

/*
 * data structure of gate
 */
class Gate {
public:
    std::string footprint;
    std::string name;

    // string store the variable name of the net that connect to this pin
    std::map<std::string, Pin *> inputPin;
    std::map<std::string, Pin *> outputPin;
    std::vector<std::string> inputNetName;
    std::vector<std::string> outputNetName;

    bool visited = false;
    double cellDelay = 0.0;
    double outputTransition = 0.0;
    double currentMaxDelay = 0.0;
    std::string maxDelayPrecedingGateName;

    Gate(std::string footprint): footprint(footprint) {}

    void deleteClass() {
        for (auto &ipin: inputPin) {
            delete ipin.second;
        }
        for (auto &opin: outputPin) {
            delete opin.second;
        }
    }

private:
};

/*
 * data structure of net
 */
class Net {
public:
    std::string name;
    int signal = -1;
    int type = -1;  // 0 for input, 1 for output, 2 for internal wire

    // if this net is input/output of the gates
    // the variable name of the net will also be
    // recorded in outputNetName of the gate and
    // inputPin/outputPin
    std::vector<std::string> inputGateName;
    std::vector<std::string> outputGateName;

    Net(std::string s): name(s){}

private:
};

/*
 * data structure of a circuit
 */
class Circuit {
public:
    std::vector<std::string> circuitGateName;
    std::vector<std::string> inputNetName;
    std::vector<std::string> outputNetName;
    std::vector<std::string> wireNetName;
    std::map<std::string, Gate *> circuitGate;
    std::map<std::string, Net *> allNet;  // this map contains all input, output, wire

    // longest delay path for a single input pattern
    double maxDelay;
    std::stack<std::string> longestPath;

    ~Circuit() {
        for (auto &gate: circuitGate) {
            gate.second->deleteClass();
        }
        for (auto &net: allNet) {
            delete net.second;
        }
    }

private:
};

#endif
