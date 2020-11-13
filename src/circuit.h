#ifndef _CIRCUIT_H_
#define _CIRCUIT_H_

#include <string>
#include <vector>
#include <map>

/*
 * data structure of pin
 */
class Pin {
public:
    std::string footprint;
    int direction = -1;  // 0 for input, 1 for output, -1 for uninitialized
    float capacitance = 0.0;
    std::string name;  // net variable name

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
    std::vector<Pin *> input;
    std::vector<Pin *> output;
    std::map<std::pair<float, float>, float> rise_power;
    std::map<std::pair<float, float>, float> fall_power;
    std::map<std::pair<float, float>, float> cell_rise;
    std::map<std::pair<float, float>, float> cell_fall;
    std::map<std::pair<float, float>, float> rise_transition;
    std::map<std::pair<float, float>, float> fall_transition;

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

    Net() {}
private:
};

/*
 * each node of the list is a cell in
 * cell library or a gate in benchmarks
 */
struct Node {
    Gate *gate = NULL;
    Node *next = NULL;
    Node(Gate* gate): gate(gate) {}
};

#endif
