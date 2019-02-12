//
// Created by Realopt on 2019-02-12.
//

#include "MinimizeTardyJobsModel.h"

#include <iostream>

// Abstract Model
MinimizeTardyJobsModel::MinimizeTardyJobsModel(const Instance &instance) : _instance(instance) {}

void MinimizeTardyJobsModel::build_model() {
    if (_instance.verbose()) cout << "Building model" << endl << "Creating x variables" << endl;
    create_x_variables();

    if (_instance.verbose()) cout << "Creating t variables" << endl;
    create_t_variables();

    if (_instance.verbose()) cout << "Creating objective" << endl;
    create_objective();

    if (_instance.verbose()) cout << "Creating one selection per group constraint" << endl;
    create_one_selection_per_group_constraint();

    if (_instance.verbose()) cout << "Creating custom constraints" << endl;
    create_other_constraints();
}

void MinimizeTardyJobsModel::create_x_variables() {

}

void MinimizeTardyJobsModel::create_t_variables() {}

void MinimizeTardyJobsModel::create_objective() {}

void MinimizeTardyJobsModel::create_one_selection_per_group_constraint() {}


// Model A
MinimizeTardyJobsWithModelA::MinimizeTardyJobsWithModelA(const Instance &instance) : MinimizeTardyJobsModel(instance) {}

void MinimizeTardyJobsWithModelA::create_other_constraints() {}
