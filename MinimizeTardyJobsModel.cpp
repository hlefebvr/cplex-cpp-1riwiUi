//
// Created by Realopt on 2019-02-12.
//

#include "MinimizeTardyJobsModel.h"

#include <iostream>

// Abstract Model
MinimizeTardyJobsModel::MinimizeTardyJobsModel(const Instance &instance) : _instance(instance) {}

void MinimizeTardyJobsModel::build_model() {
    if (_instance.verbose()) cout << "Building model" << endl;

    const vector<const JobOccurence*>& occurences = _instance.occurences();

    IloExpr objective = IloExpr(_env);
    vector<IloExpr> assignment_constraint;
    assignment_constraint.assign(_instance.jobs().size(), IloExpr(_env));
    for (unsigned long int k = 0, n = occurences.size() ; k < n ; k += 1) {
        const JobOccurence& job_occ = *occurences.at(k);

        // create variable x_k and t_k
        _x.add(IloBoolVar(_env));
        _t.add(IloNumVar(_env));

        // append valuation in objective
        objective += job_occ._parent_job._weight * _x[k];

        // group occurences by job
        assignment_constraint[job_occ._parent_job._id] += _x[k];
    }

    // add objective
    _constraints.add(IloMinimize(_env, objective));

    // create constraints per group
    for (const IloExpr& constraint : assignment_constraint)
        _constraints.add(constraint == 1);

    if (_instance.verbose()) cout << "Creating custom constraints" << endl;
    create_other_constraints();
}

// Model A
MinimizeTardyJobsWithModelA::MinimizeTardyJobsWithModelA(const Instance &instance) : MinimizeTardyJobsModel(instance) {}

void MinimizeTardyJobsWithModelA::create_other_constraints() {}
