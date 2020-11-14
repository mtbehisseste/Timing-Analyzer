#ifndef _CIRCUIT_H_
#define _CIRCUIT_H_

#include <string>
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
 * data structure of gate
 */
class Gate {
public:
    std::string footprint;
    std::string name;
    std::map<std::string, Pin *> inputPin;
    std::map<std::string, Pin *> outputPin;
    std::map<std::string, Net *> inputNet;
    std::map<std::string, Net *> outputNet;
    std::map<std::pair<float, float>, float> rise_power;
    std::map<std::pair<float, float>, float> fall_power;
    std::map<std::pair<float, float>, float> cell_rise;
    std::map<std::pair<float, float>, float> cell_fall;
    std::map<std::pair<float, float>, float> rise_transition;
    std::map<std::pair<float, float>, float> fall_transition;
    
    bool visited = false;

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
    // if this net is input/output of the gates
    // the variable name of the net will also be
    // recorded in pin->name of the gate
    std::map<std::string, Gate *> inputGate;
    std::map<std::string, Gate *> outputGate;

    Net(std::string s): name(s){}
private:
};

/*
 * data structure of a circuit
 */
class Circuit {
public:
    std::map<std::string, Gate *> circuitGate;
    std::map<std::string, Net *> inputNet;
    std::map<std::string, Net *> outputNet;
    std::map<std::string, Net *> wireNet;
    std::map<std::string, Net *> allNet;  // this map contains all input, output, wire
private:
};

#endif
