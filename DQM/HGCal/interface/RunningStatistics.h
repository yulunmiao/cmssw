#ifndef __RUNNING_STATISTICS__
#define __RUNNING_STATISTICS__

#include <stdio.h>
#include <cmath>

struct statistics {
    double nevents;
    double mean_x;
    double mean_y;
    double variance_x;
    double variance_y;
    double covariance;
    double correlation;
    double slope;
    double intercept;
};

class RunningStatistics {
    public:
        RunningStatistics();
        ~RunningStatistics(){};

        void add_entry(double x, double y);
        void print_statistics();
        double get_correlation();
        double get_slope();
        double get_intercept();
        double get_mean_adc()    { return current.mean_y;      }

    private:
        statistics current;
        statistics next;
};

RunningStatistics::RunningStatistics()
{
    // state variables
    current.nevents       = 0.;
    current.mean_x        = 0.;
    current.mean_y        = 0.;
    current.variance_x    = 0.;
    current.variance_y    = 0.;
    current.covariance    = 0.;
    current.correlation   = 0.;
    current.slope         = 0.;
    current.intercept     = 0.;

    next.nevents       = 0 ;
    next.mean_x        = 0.;
    next.mean_y        = 0.;
    next.variance_x    = 0.;
    next.variance_y    = 0.;
    next.covariance    = 0.;
    next.correlation   = 0.;
    next.slope         = 0.;
    next.intercept     = 0.;
}

void RunningStatistics::add_entry(double x, double y)
{
    // evaluate
    next.nevents     = current.nevents + 1.;
    next.mean_x      = (current.nevents*current.mean_x)/next.nevents + x/next.nevents;
    next.mean_y      = (current.nevents*current.mean_y)/next.nevents + y/next.nevents;
    next.variance_x  = current.variance_x*current.nevents/next.nevents + std::pow(x,2)/next.nevents + std::pow(current.mean_x,2)*current.nevents/next.nevents - std::pow(next.mean_x,2);
    next.variance_y  = current.variance_y*current.nevents/next.nevents + std::pow(y,2)/next.nevents + std::pow(current.mean_y,2)*current.nevents/next.nevents - std::pow(next.mean_y,2);
    next.covariance  = current.covariance*current.nevents/next.nevents + x*y/next.nevents + current.mean_x*current.mean_y*current.nevents/next.nevents - next.mean_x*next.mean_y;
    next.correlation = (next.variance_x>0. && next.variance_y>0.) ? next.covariance / (std::sqrt(next.variance_x)*std::sqrt(next.variance_y)) : 0.;
    next.slope       = (next.variance_x>0. && next.variance_y>0.) ? next.correlation * std::sqrt(next.variance_y) / std::sqrt(next.variance_x) : 0.;
    next.intercept   = (next.variance_x>0. && next.variance_y>0.) ? next.mean_y - next.slope*next.mean_x : 0.;

    // update
    current = next;
}

double RunningStatistics::get_correlation() { return current.correlation; }
double RunningStatistics::get_slope()       { return current.slope;       }
double RunningStatistics::get_intercept()   { return current.intercept;   }

void RunningStatistics::print_statistics()
{
    printf("current.nevents = %.3f\n"     , current.nevents     );
    printf("current.mean_x = %.3f\n"      , current.mean_x      );
    printf("current.mean_y = %.3f\n"      , current.mean_y      );
    printf("current.variance_x = %.3f\n"  , current.variance_x  );
    printf("current.variance_y = %.3f\n"  , current.variance_y  );
    printf("current.covariance = %.3f\n"  , current.covariance  );
    printf("current.correlation = %.3f\n" , current.correlation );
    printf("current.slope = %.3f\n"       , current.slope       );
    printf("current.intercept = %.3f\n"   , current.intercept   );
    printf("\n");
}

#endif
