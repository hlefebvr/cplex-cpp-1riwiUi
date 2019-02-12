#include <string>

#include "Instance.h"
#include "MinimizeTardyJobsModel.h"

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

    MinimizeTardyJobsWithModelA solver_A = MinimizeTardyJobsWithModelA(instance);
    solver_A.build_model();
    solver_A.solve();

    /* cout << endl << endl;

    MinimizeTardyJobsWithModelMMKP solver_MMKP = MinimizeTardyJobsWithModelMMKP(instance);
    solver_MMKP.build_model();
    solver_MMKP.solve(); */

    return 0;
}
