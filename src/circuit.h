#ifndef _CIRCUIT_H_
#define _CIRCUIT_H_

#include <string>

/*
 * data structure of gate
 */
class Gate {
public:
    std::string name;

    Gate() {}

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
    Gate gate;
    Node *next;
};

#endif
