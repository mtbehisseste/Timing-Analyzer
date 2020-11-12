#include "circuit.h"
#include "readFile.h"

int main() {
    // argparse
    
    std::string libName = "../lib/test_lib.lib";
    
    Node *libHead;
    readLibrary(libHead, libName);

    return 0;
}
