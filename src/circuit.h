#ifndef _CIRCUIT_H_
#define _CIRCUIT_H_

#include <string>
#include <vector>
#include <map>

class Net;

/*
 * data structure of pin
 */
class Pin {
public:
    std::string footprint;
    int direction = -1;  // 0 for input, 1 for output, -1 for uninitialized
    float capacitance = 0.0;

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
    std::vector<float> index1;
    std::vector<float> index2;
    std::map<std::pair<float, float>, float> rise_power;
    std::map<std::pair<float, float>, float> fall_power;
    std::map<std::pair<float, float>, float> cell_rise;
    std::map<std::pair<float, float>, float> cell_fall;
    std::map<std::pair<float, float>, float> rise_transition;
    std::map<std::pair<float, float>, float> fall_transition;

    Cell(std::string footprint): footprint(footprint) {}

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
    float cellDelay = 0.0;
    float outputTransition = 0.0;
    float currentMaxDelay = 0.0;
    std::string maxDelayPrecedingNetName;

    Gate(std::string footprint): footprint(footprint) {}

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
private:
};

#endif
