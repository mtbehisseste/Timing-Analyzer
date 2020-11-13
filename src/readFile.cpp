#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstring>

#include "readFile.h"

using namespace std;

void readLibrary(Node *&libHead, string libName)
{
    fstream fLib;
    fLib.open(libName.c_str(), ios::in);
    if (!fLib) {
        cout << "Error opening library file\n";
        exit(-1);
    }

    char line[1000];
    string strline;
    size_t first_quote, last_quote;

    // skip unit attributes
    for (int i = 0; i < 9; ++i)
        fLib.getline(line, 1000);

    // record output net capacitances and input transition time
    float outputCapacitance[7], inputTransitionTime[7];

    fLib.getline(line, 1000);  // index 1
    strline = string(line);
    first_quote = strline.find_first_of("\"");
    last_quote = strline.find_last_of("\"");
    strline = strline.substr(first_quote + 1, last_quote-first_quote - 1);
    sscanf(strline.c_str(), "%f,%f,%f,%f,%f,%f,%f", &outputCapacitance[0],
            &outputCapacitance[1], &outputCapacitance[2], &outputCapacitance[3],
            &outputCapacitance[4], &outputCapacitance[5], &outputCapacitance[6]);

    memset(line, 0, 1000);
    fLib.getline(line, 1000);  // index 2
    strline = string(line);
    first_quote = strline.find_first_of("\"");
    last_quote = strline.find_last_of("\"");
    strline = strline.substr(first_quote + 1, last_quote-first_quote - 1);
    sscanf(strline.c_str(), "%f,%f,%f,%f,%f,%f,%f", &inputTransitionTime[0],
            &inputTransitionTime[1], &inputTransitionTime[2], &inputTransitionTime[3],
            &inputTransitionTime[4], &inputTransitionTime[5], &inputTransitionTime[6]);

}
