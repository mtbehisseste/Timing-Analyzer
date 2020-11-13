#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstring>

#include "readFile.h"

using namespace std;

void readLibrary(Node *&libHead, string libName)
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

    // skip "}"
    getline(fLib, strline);

}
