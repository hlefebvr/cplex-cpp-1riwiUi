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
    const double _release;
    const double _deadline;
    const double  _weight;
    const double _processing_time;
    const unsigned int _parent_job_id;

    friend ostream& operator<<(ostream& os, const JobOccurence& x);

    explicit JobOccurence(unsigned int parent_id, double release, double deadline, double weight, double processing_time)
        : _parent_job_id(parent_id), _release(release), _deadline(deadline), _weight(weight), _processing_time(processing_time) {}
};

/**
 * Instance loader class
 */
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
