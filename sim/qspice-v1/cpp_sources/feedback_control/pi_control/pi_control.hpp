#ifndef PICONTROLLER_H
#define PICONTROLLER_H

#include <cmath>

class pi_control {
public:
    pi_control(double kp, double ki, double pi_limit);
    double calculate_pi_out(double error, double lower_limit, double upper_limit);

protected:
    double sgn(double input);

    double kp;
    double ki;
    double i_term;
    double pi_limit;
};

#endif // PICONTROLLER_H

