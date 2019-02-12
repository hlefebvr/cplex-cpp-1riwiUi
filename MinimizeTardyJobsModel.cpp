//
// Created by Realopt on 2019-02-12.
//

#include "MinimizeTardyJobsModel.h"

#include <iostream>
#include <fstream>

// Abstract Model
MinimizeTardyJobsModel::MinimizeTardyJobsModel(const Instance &instance, bool with_t) : _instance(instance), _with_t(with_t) {}

void MinimizeTardyJobsModel::build_model() {
    if (_instance.verbose()) cout << "Building model" << endl;

    const vector<const JobOccurence*>& occurences = _instance.occurences();

    IloExpr objective = IloExpr(_env);
    vector<IloExpr> assignment_constraint;
    for (unsigned long int i = 0, n = _instance.jobs().size() ; i < n ; i += 1)
        assignment_constraint.emplace_back(IloExpr(_env));
    for (unsigned long int k = 0, n = occurences.size() ; k < n ; k += 1) {
        const JobOccurence& job_occ = *occurences.at(k);

        // create variable x_k and t_k
        _x.add(IloBoolVar(_env));
        if (_with_t) _t.add(IloNumVar(_env));

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

void MinimizeTardyJobsModel::solve() {
    IloCplex cplex(_constraints);
    ofstream logout = ofstream("cplex.log", ios::out);

    cplex.setOut(logout);
    cplex.exportModel("lpex1.lp");

    if (_instance.verbose()) cout << "Launching Cplex solver..." << endl;
    cplex.solve();

    cout << "Status = " << cplex.getStatus() << endl;
    if (cplex.getStatus() == IloAlgorithm::Optimal) {

        cout << "Objective value = " << cplex.getObjValue() << endl;

        IloNumArray vals(_env);

        cplex.getValues(vals, _x);
        cout << "x = " << vals << endl;

        if (_with_t) {
            cplex.getValues(vals, _t);
            cout << "t = " << vals << endl;
        }
    }

    logout.close();
}

// Model A
MinimizeTardyJobsWithModelA::MinimizeTardyJobsWithModelA(const Instance &instance) : MinimizeTardyJobsModel(instance) {}

void MinimizeTardyJobsWithModelA::create_other_constraints() {
    const vector<const JobOccurence*>& occurences = _instance.occurences();

    for (unsigned long int k = 0, n = occurences.size() ; k < n ; k += 1) {
        const JobOccurence &job_occ = *occurences.at(k);

        _constraints.add( _t[k] - (job_occ._from + job_occ._parent_job._processing_time) * _x[k] >= 0 );

        _constraints.add( _t[k] <= job_occ._to );

        if (k == 0) continue;
        _constraints.add( _t[k] - _t[k-1] - job_occ._parent_job._processing_time * _x[k] >= 0 );
    }
}

MinimizeTardyJobsWithModelMMKP::MinimizeTardyJobsWithModelMMKP(const Instance &instance) : MinimizeTardyJobsModel(instance, false) {}

void MinimizeTardyJobsWithModelMMKP::create_other_constraints() {
    const vector<const JobOccurence*>& occurences = _instance.occurences();

    for (unsigned long int k = 0, n = occurences.size() ; k < n ; k += 1) {
        const JobOccurence &job_k = *occurences.at(k);
        for (unsigned long int l = k ; l < n ; l += 1) {
            const JobOccurence &job_l = *occurences.at(l);

            IloExpr constraint = IloExpr(_env);

            for (unsigned long int kk = k + 1 ; kk < n ; kk += 1) {
                const JobOccurence &job_kk = *occurences.at(l);
                constraint += job_kk._parent_job._processing_time * _x[kk];
            }

            constraint += (job_k._from + job_k._parent_job._processing_time) * _x[k];

            _constraints.add( constraint <= job_l._to );
        }
    }
}
