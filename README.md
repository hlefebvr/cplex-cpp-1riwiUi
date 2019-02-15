# Cplex models for minimizing the weighted number of tardy jobs

- 1|r_i|\sum w_i U_i (NP-hard)
- All models refer to [this article from B. Detienne (INRIA)](https://hal.inria.fr/hal-00880908/)

‘‘‘shell
MinimizeTardyJobs <instance_filepath> [-v]
‘‘‘

verbose : verbose mode and exports CPLEX model in ‘<model_name>.lp‘