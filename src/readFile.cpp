#include <fstream>
#include <iostream>
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
    size_t first_quote, last_quote;
    // skip unit attributes
    for (int i = 0; i < 9; ++i)
        fLib.getline(line, 1000);

    // record output net capacitances and input transition time
    float outputCapacitance[7], inputTransitionTime[7];
    string strline, subline, tok;
    int cntTok = 0, commaIndex = 0;

    fLib.getline(line, 1000);  // index 1
    strline = string(line);
    first_quote = strline.find_first_of("\"");
    last_quote = strline.find_last_of("\"");
    subline = strline.substr(first_quote + 1, last_quote-first_quote - 1);
    commaIndex = subline.find(",");
    tok = subline.substr(0, commaIndex);
    subline = subline.substr(commaIndex + 1);
    while (commaIndex <= subline.length()) {
        outputCapacitance[cntTok++] = stof(tok);
        commaIndex = subline.find(",");
        tok = subline.substr(0, commaIndex);
        subline = subline.substr(commaIndex + 1);
    }
    outputCapacitance[cntTok] = stof(tok);

    cntTok = 0;
    memset(line, 0, 1000);
    fLib.getline(line, 1000);  // index 1
    strline = string(line);
    first_quote = strline.find_first_of("\"");
    last_quote = strline.find_last_of("\"");
    subline = strline.substr(first_quote + 1, last_quote-first_quote - 1);
    commaIndex = subline.find(",");
    tok = subline.substr(0, commaIndex);
    subline = subline.substr(commaIndex + 1);
    while (commaIndex <= subline.length()) {
        inputTransitionTime[cntTok++] = stof(tok);
        commaIndex = subline.find(",");
        tok = subline.substr(0, commaIndex);
        subline = subline.substr(commaIndex + 1);
    }
    inputTransitionTime[cntTok] = stof(tok);

}
