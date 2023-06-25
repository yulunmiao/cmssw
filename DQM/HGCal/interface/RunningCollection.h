#ifndef __RUNNING_COLLECTION__
#define __RUNNING_COLLECTION__
#include <vector>
#include "RunningStatistics.h"

class RunningCollection{
    public:
        RunningCollection();
        ~RunningCollection();

        void add_entry(int channleId, double value, double cm);
        std::vector<RunningStatistics> get_vector_running_statistics() {return mVecRunningStatistics;};
        void write_csv_file();

    private:
        int Nchannels;
        std::vector<RunningStatistics> mVecRunningStatistics;

};

RunningCollection::RunningCollection()
{
    Nchannels = 234;

    for(int i=0; i<Nchannels; ++i)
    {
        RunningStatistics instance;
        mVecRunningStatistics.push_back(instance);
    }
}

RunningCollection::~RunningCollection() {}

void RunningCollection::add_entry(int channleId, double value, double cm)
{
    if(channleId>=234) {
        printf("[ERROR] RunningCollection::add_entry: the channel ID is out of scope\n");
        return;
    }

    mVecRunningStatistics[channleId].add_entry(cm, value);
}

void RunningCollection::write_csv_file() {};

#endif
