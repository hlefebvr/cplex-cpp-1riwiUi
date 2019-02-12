#include <string>
#include <ilcplex/ilocplex.h>

#include "Instance.h"

using namespace std;

int help(int return_code = -1) {
    cout << "Usage : main <instance_file> [-v]" << endl;
    return return_code;
}

int main(int argc, char** argv) {
    if (argc < 2) return help();

    const string instance_filename = string(argv[1]);
    const bool verbose = argc == 3 && string(argv[2]) == "-v";

    Instance instance  = Instance(instance_filename, verbose);

    return 0;
}
