//
// Created by Realopt on 2019-02-12.
//

#include <algorithm>
#include <regex>
#include <iostream>
#include <fstream>

#include "Instance.h"

ostream& operator<<(ostream& os, const Job& x) {
    return os << "job " << x._id << " [" << x._release_date << ", " << x._deadline << "], p = " << x._processing_time << ", w = " << x._weight;
}

ostream &operator<<(ostream &os, const JobOccurence&  x) {
    return os << "job occurence from " << x._release << " to " << x._deadline<< " (comming from job " << x._parent_job_id << ")";
}

Instance::Instance(const string &filename, bool verbose) : _instance_filename(filename), _verbose(verbose) {
    load_jobs_from_instance();
    build_occurences_from_jobs();
    apply_edf_rule();

    if (_verbose) cout << endl;
}

vector<string> Instance::get_next_row(ifstream& reader) const {
    char* placeholder = new char[MAX_LINE_SIZE];
    reader.getline(placeholder, MAX_LINE_SIZE);
    string line(placeholder);

    regex reg = regex("[[:alnum:][:punct:]]+");
    smatch match;

    vector<string> cols;
    while( regex_search(line, match, reg) ) {
        cols.push_back(match[0]);
        line = match.suffix().str();
    }

    delete[] placeholder;

    return cols;
}

void Instance::load_jobs_from_instance() {
    if(_verbose) cout << "Loading jobs from instance file :\n---------------------------------" << endl;

    enum COLUMN { WEIGHT, RELEASE_DATE, DEADLINE, PROCESSING_TIME };

    ifstream reader = ifstream(_instance_filename, ios::in);
    if (reader.fail()) throw runtime_error("Could not open instance file");

    get_next_row(reader); // skip the header

    unsigned int id = 0;
    while (!reader.eof()) {
        auto row = get_next_row(reader);

        if (row.size() == 4) {
            const int weight = stoi(row[COLUMN::WEIGHT]);
            const int release_date = stoi(row[COLUMN::RELEASE_DATE]);
            const int deadline = stoi(row[COLUMN::DEADLINE]);
            const int processing_time = stoi(row[COLUMN::PROCESSING_TIME]);

            const Job *new_job = new Job(id, release_date, deadline, weight, processing_time);
            if (_verbose) cout << "Creating " << *new_job << endl;
            _jobs.emplace_back(new_job);

            id += 1;
        }
    }

    reader.close();
}

void Instance::build_occurences_from_jobs() {
    if (_verbose) cout << "Sorting jobs by release date" << endl;
    sort(_jobs.begin(), _jobs.end(), [](const Job* A, const Job* B) { return A->_release_date < B->_release_date; });

    for (unsigned long int i = 0, n_jobs = _jobs.size() ; i < n_jobs ; i += 1) {
        const Job& job_i = *_jobs[i];
        if(_verbose) cout << "Creating job occurences for job " << job_i << endl;
        if (_max_deadline < job_i._deadline) _max_deadline = job_i._deadline;

        for (unsigned long int j = i + 1 ; j < n_jobs ; j += 1) {
            const Job& job_j = *_jobs[j];

            if (job_i._deadline > job_j._deadline && job_i._release_date + job_i._processing_time + job_j._processing_time < job_j._deadline) {
                const int from = job_i._release_date;
                const int to = job_j._deadline;
                auto new_occurence = new JobOccurence(job_i._id, from, to, 0, job_i._processing_time);

                if (_verbose) cout << "\tCreating " << *new_occurence << endl;
                _occurences.emplace_back(new_occurence);
            } else if (job_i._deadline <= job_j._release_date) {
                break;
            }
        }

        auto new_occurence = new JobOccurence(job_i._id, job_i._release_date, job_i._deadline, 0, job_i._processing_time);
        if (_verbose) cout << "\tCreating default " << *new_occurence << endl;
        _occurences.emplace_back(new_occurence);

        new_occurence = new JobOccurence(job_i._id, 0, 0, job_i._weight, 0);
        if (_verbose) cout << "\tCreating tardy fictious " << *new_occurence << endl;
        _occurences.emplace_back(new_occurence);
    }

    if (_verbose) cout << "Done. (" + to_string(_occurences.size()) + " occurences have been generated)" << endl;
}

void Instance::apply_edf_rule() {
    sort(_occurences.begin(), _occurences.end(), [](const JobOccurence* A, const JobOccurence* B){
        return A->_deadline < B->_deadline;
    });
}

Instance Instance::reverse(const Instance& instance) {
    return Instance(instance, true);
}

Instance::Instance(const Instance& instance, bool) : _instance_filename(instance._instance_filename), _verbose(instance._verbose) {
    const int dmax = instance._max_deadline;

    for (const Job* job : instance._jobs)
        _jobs.emplace_back(new Job(job->_id, dmax - job->_deadline, dmax - job->_release_date, job->_weight, job->_processing_time));

    for (const JobOccurence* job_occ : instance._occurences)
        _occurences.emplace_back(new JobOccurence(job_occ->_parent_job_id, dmax - job_occ->_deadline, dmax - job_occ->_release, job_occ->_weight, job_occ->_processing_time));

    _max_deadline = dmax - _occurences.front()->_release;
}
