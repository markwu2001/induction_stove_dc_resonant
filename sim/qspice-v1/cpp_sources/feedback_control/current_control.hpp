#ifndef CURRENTCONTROLLER_H
#define CURRENTCONTROLLER_H

#include <cmath>

class CurrentController {
public:
    CurrentController(double ikp, double iki, double min_duty = 0.1, double max_duty = 0.9, double pi_limit = 31.0);

    double compute(double iref, double sampled_current, double vin, double uout);

    void setMinDuty(double min_duty);
    void setMaxDuty(double max_duty);
    void setPiLimit(double pi_limit);

private:
    double ikp;
    double iki;
    double i_term;
    double min_duty;
    double max_duty;
    double pi_limit;

    double calculate_pi_out(double i_error, double lower_limit, double upper_limit);
    double sgn(double input);
};

#endif // CURRENTCONTROLLER_H

