//
// Created by Realopt on 2019-02-12.
//

#ifndef DETERMINISTIC_MINIMIZETARDYJOBSMODEL_H
#define DETERMINISTIC_MINIMIZETARDYJOBSMODEL_H

#include <ilcplex/ilocplex.h>

#include "Instance.h"

class MinimizeTardyJobsModel {
protected:
    const Instance& _instance;
    const bool _with_t;
    IloEnv _env;
    IloModel _constraints = IloModel(_env, "Minimize number of tardy jobs");
    IloBoolVarArray _x = IloBoolVarArray(_env);
    IloNumVarArray _t = IloNumVarArray(_env);

    virtual void create_other_constraints() = 0;
public:
    explicit MinimizeTardyJobsModel(const Instance& instance, bool with_t = true);
    void build_model();
    void solve();
};

class MinimizeTardyJobsWithModelA : public MinimizeTardyJobsModel {
protected:
    void create_other_constraints() override;
public:
    explicit MinimizeTardyJobsWithModelA(const Instance& instance);
};

class MinimizeTardyJobsWithModelMMKP : public MinimizeTardyJobsModel {
protected:
    void create_other_constraints() override;
public:
    explicit MinimizeTardyJobsWithModelMMKP(const Instance& instance);
};


#endif //DETERMINISTIC_MINIMIZETARDYJOBSMODEL_H
