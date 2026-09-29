#include "deadtimecontroller.hpp"

DeadtimeController::DeadtimeController(double gate_hi_voltage, double gate_lo_voltage, double deadtime)
    : gate_hi_voltage(gate_hi_voltage), gate_lo_voltage(gate_lo_voltage), deadtime(deadtime),
      previous_pwm(gate_lo_voltage), PWM_hi(gate_lo_voltage), PWM_lo(gate_hi_voltage),
      deadtime_start(0.0), deadtime_stop(0.0) {}

void DeadtimeController::update(double t, double pwm) {
    if (pwm > 0) {
        PWM_hi = gate_hi_voltage;
        PWM_lo = gate_lo_voltage;
    } else {
        PWM_hi = gate_lo_voltage;
        PWM_lo = gate_hi_voltage;
    }

    if (previous_pwm != pwm) {
        deadtime_start = t;
        deadtime_stop = t + deadtime;
    }
    previous_pwm = pwm;

    applyDeadtime(t);
}

void DeadtimeController::applyDeadtime(double t) {
    if (t >= deadtime_start && t <= deadtime_stop) {
        PWM_hi = gate_lo_voltage;
        PWM_lo = gate_lo_voltage;
    }
}

double DeadtimeController::getPWM_hi() const {
    return PWM_hi;
}

double DeadtimeController::getPWM_lo() const {
    return PWM_lo;
}


