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
    IloEnv env;

    void create_x_variables();
    void create_t_variables();
    void create_objective();
    void create_one_selection_per_group_constraint();
    virtual void create_other_constraints() = 0;
public:
    explicit MinimizeTardyJobsModel(const Instance& instance);
    void build_model();
};

class MinimizeTardyJobsWithModelA : public MinimizeTardyJobsModel {
protected:
    void create_other_constraints() override;
public:
    explicit MinimizeTardyJobsWithModelA(const Instance& instance);
};


#endif //DETERMINISTIC_MINIMIZETARDYJOBSMODEL_H
