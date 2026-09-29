#ifndef DEADTIMECONTROL_H
#define DEADTIMECONTROL_H
#include <cmath>

class DeadtimeController {
public:
    DeadtimeController(double gate_hi_voltage, double gate_lo_voltage, double deadtime);

    void update(double t, double pwm);
    double getPWM_hi() const;
    double getPWM_lo() const;

private:
    double gate_hi_voltage;
    double gate_lo_voltage;
    double deadtime;
    double previous_pwm;
    double PWM_hi;
    double PWM_lo;
    double deadtime_start;
    double deadtime_stop;

    void applyDeadtime(double t);
};

#endif // DEADTIMECONTROL_H
