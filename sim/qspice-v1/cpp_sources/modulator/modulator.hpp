#ifndef MODULATOR_H
#define MODULATOR_H
#include <cmath>
#include "../deadtime/deadtimecontroller.hpp"

class Modulator {
public:
    Modulator(double Ts, double duty, double gate_hi_voltage, double gate_lo_voltage, double deadtime);

    void update(double t);
    void set_duty(double set_duty_to);
    void set_phase(double set_phase_to);
    void set_frequency(double set_frequency_to);
    bool synchronous_sample_called(double t) const;

    double getPWM() const;
    double getPWMLo() const;
    double get_carrier() const;

private:

    DeadtimeController deadtimecontrol;

    double Ts;
    double duty;
    double previous_duty;
    double carrier;
    double previous_carrier;
    double phase;

    double PWM;

    int rising_edge_when_1;
    int previous_edge;
};

#endif // MODULATOR_H
