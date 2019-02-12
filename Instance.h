//
// Created by Realopt on 2019-02-12.
//

#ifndef DETERMINISTIC_INSTANCE_H
#define DETERMINISTIC_INSTANCE_H

#define MAX_LINE_SIZE 30

#include <vector>
#include <string>

using namespace std;

/**
 * Structure to store jobs
 */
struct Job {
    const int _release_date;
    const int _deadline;
    const int _processing_time;
    const int _weight;
    const unsigned int _id;

    friend ostream& operator<<(ostream& os, const Job& x);

    Job(const unsigned int id, const int release_date, const int deadline, const int processing_time, const int weight)
            : _id(id), _release_date(release_date), _deadline(deadline), _processing_time(processing_time), _weight(weight) {}
};

/**
 * Job occurence of job _parent_job from _from to _to
 */
struct JobOccurence {
    const Job& _parent_job;
    const int _from;
    const int _to;
    const bool _is_last_occurence;

    friend ostream& operator<<(ostream& os, const JobOccurence& x);

    JobOccurence(const int from, const int to, const Job& parent_job, const bool is_last_occurence = false)
            : _from(from), _to(to), _parent_job(parent_job), _is_last_occurence(is_last_occurence) {}
};

class Instance {
    const string _instance_filename;
    const bool _verbose;
    vector<const Job*> _jobs;
    vector<const JobOccurence*> _occurences;
    int _max_deadline = 0;

    void load_jobs_from_instance();
    void build_occurences_from_jobs();
    void apply_edf_rule();
    vector<string> get_next_row(ifstream& reader) const;
public:
    explicit Instance(const string& filename, bool verbose);
    const vector<const Job*>& jobs() const { return _jobs; }
    const vector<const JobOccurence*>& occurences() const { return _occurences; }
    bool verbose() const { return _verbose; }
    int max_deadline() const { return _max_deadline; }
    string instance_name() const { return _instance_filename;}
};

#endif //DETERMINISTIC_INSTANCE_H
