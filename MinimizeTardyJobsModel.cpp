//
// Created by Realopt on 2019-02-12.
//

#include "MinimizeTardyJobsModel.h"

#include <iostream>
#include <fstream>

// Abstract Model
MinimizeTardyJobsModel::MinimizeTardyJobsModel(const Instance &instance, const string& model_name, const bool with_t)
    : _instance(instance), _model_name(model_name), _with_t(with_t) {}

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

        _x.add(IloBoolVar(_env, ("x_" + to_string(k)).c_str()));
        if(_with_t) _t.add(IloNumVar(_env, ("t_" + to_string(k)).c_str()));

        // append valuation in objective
        objective += job_occ._weight * _x[k];

        // group occurences by job
        assignment_constraint[job_occ._parent_job_id] += _x[k];
    }

    // add objective
    _constraints.add(IloMinimize(_env, objective)).setName("objective");

    // create constraints per group
    unsigned int i = 0;
    for (const IloExpr& constraint : assignment_constraint) {
        _constraints.add(constraint == 1).setName(("one_among_group_" + to_string(i)).c_str());
        i += 1;
    }

    if (_instance.verbose()) cout << "Creating custom constraints" << endl;
    create_other_constraints();
}

void MinimizeTardyJobsModel::solve() {
    IloCplex cplex(_constraints);
    ofstream logout = ofstream((_model_name + ".log").c_str(), ios::out);

    cplex.setOut(logout);
    if(_instance.verbose()) {
        cout << "Exporting cplex model..." << endl;
        cplex.exportModel((_model_name + ".lp").c_str());
    }

    if (_instance.verbose()) cout << "Launching Cplex solver..." << endl;
    cplex.solve();

    cout << "Status = " << cplex.getStatus() << endl;
    if (cplex.getStatus() == IloAlgorithm::Optimal) {

        cout << "Objective value = " << cplex.getObjValue() << endl;

        IloNumArray x = IloNumArray(_env);
        IloNumArray t = IloNumArray(_env);

        cplex.getValues(x, _x);
        if(_with_t) cplex.getValues(t, _t);

        const vector<const JobOccurence*>& occurences = _instance.occurences();
        vector<unsigned int> tardy_ids;

        for (unsigned long int i = 0, n = occurences.size() ; i < n ; i += 1) {
            if (x[i] == 1) {
                const JobOccurence& job_occ = *occurences.at(i);
                if (job_occ._processing_time > 0) {
                    if (_with_t) {
                        const int from = t[i] - job_occ._processing_time;
                        const int to = t[i];
                        cout << "job " << job_occ._parent_job_id << " from " << t[i] - job_occ._processing_time
                             << " to " << t[i] << ", p = " << job_occ._processing_time << endl;
                    } else {
                        cout << "job " << job_occ._parent_job_id << endl;
                    }
                } else {
                    tardy_ids.emplace_back(job_occ._parent_job_id);
                }
            }
        }

        cout << "Tardy jobs : (";
        for (unsigned int id : tardy_ids) cout << id << ", ";
        cout << ")" << endl;
    }

    logout.close();
}

// Model A
MinimizeTardyJobsWithModelA::MinimizeTardyJobsWithModelA(const Instance &instance) : MinimizeTardyJobsModel(instance, "model_A") {}

void MinimizeTardyJobsWithModelA::create_other_constraints() {
    const vector<const JobOccurence*>& occurences = _instance.occurences();

    for (unsigned long int k = 0, n = occurences.size() ; k < n ; k += 1) {
        const JobOccurence &job_occ = *occurences.at(k);

        _constraints.add( _t[k] - (job_occ._release+ job_occ._processing_time) * _x[k] >= 0 ).setName(("release_date_" + to_string(k)).c_str());

        _constraints.add( _t[k] <= job_occ._deadline ).setName(("deadline_" + to_string(k)).c_str());

        if (k == 0) continue;
        _constraints.add( _t[k] - _t[k-1] - job_occ._processing_time * _x[k] >= 0 ).setName(("overlap_" + to_string(k)).c_str());
    }
}

// Model MMKP
MinimizeTardyJobsWithModelMMKP::MinimizeTardyJobsWithModelMMKP(const Instance &instance) : MinimizeTardyJobsModel(instance, "model_MMKP", false) {}

void MinimizeTardyJobsWithModelMMKP::create_other_constraints() {
    const vector<const JobOccurence*>& occurences = _instance.occurences();

    for (unsigned long int k = 0, n = occurences.size() ; k < n ; k += 1) {
        const JobOccurence &job_k = *occurences.at(k);
        for (unsigned long int l = k ; l < n ; l += 1) {
            const JobOccurence &job_l = *occurences.at(l);

            IloExpr constraint = IloExpr(_env);

            for (unsigned long int kk = k + 1 ; kk <= l ; kk += 1) {
                const JobOccurence &job_kk = *occurences.at(kk);
                constraint += job_kk._processing_time * _x[kk];
            }

            constraint += (job_k._release + job_k._processing_time) * _x[k];

            _constraints.add( constraint <= job_l._deadline).setName(("MMKP_" + to_string(k) + "_" + to_string(l)).c_str());
        }

    }
}


// Model B
MinimizeTardyJobsWithModelB::MinimizeTardyJobsWithModelB(const Instance &reversed_instance) : MinimizeTardyJobsModel(reversed_instance, "model_B") {
    const vector<const JobOccurence*>& occurences = _instance.occurences();

    for (unsigned long int k = 0, n = occurences.size() ; k < n ; k += 1) {
        const JobOccurence &job_k = *occurences.at(k);

        int maximum = 0;
        for (unsigned long int l = k + 1, n = occurences.size() ; l < n ; l += 1) {
            const JobOccurence &job_l = *occurences.at(l);
            maximum = max(maximum, job_l._deadline - job_k._deadline);
        }
        _M.emplace_back( max(0, maximum) );
    }
}

void MinimizeTardyJobsWithModelB::create_other_constraints() {
    const vector<const JobOccurence*>& occurences = _instance.occurences();

    for (unsigned long int k = 0, n = occurences.size() ; k < n ; k += 1) {
        const JobOccurence &job_k = *occurences.at(k);

        _constraints.add( _t[k] >= job_k._release );

        _constraints.add( _t[k] + job_k._processing_time * _x[k] - _M[k] * (1 - _x[k]) <= job_k._deadline );

        if (k == 0) continue;
        _constraints.add( _t[k-1] - _t[k] - job_k._processing_time * _x[k] >= 0 );
    }
}
