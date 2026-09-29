#include "modulator.hpp"
#include "../carrier_generation/carrier_generation.hpp"

Modulator::Modulator(double Ts, double duty, double gate_hi_voltage, double gate_lo_voltage, double deadtime)
    : deadtimecontrol(gate_hi_voltage, gate_lo_voltage, deadtime), Ts(Ts),
    duty(duty), previous_duty(0.0), carrier(0.0), previous_carrier(0.0), phase(0.0), rising_edge_when_1(0), previous_edge(0) { }


void Modulator::update(double t) {

    previous_carrier = carrier;
    carrier          = calculate_carrier(t, Ts, phase);
    previous_edge    = rising_edge_when_1;
    if (previous_carrier > carrier) {
        rising_edge_when_1 = 1.0;
    }else
        rising_edge_when_1 = 0.0;

    if (duty > carrier) {
        PWM = 1.0;
    } else {
        PWM = 0.0;
    }
    deadtimecontrol.update(t, PWM);
}


void Modulator::set_duty(double set_duty_to)
{
    previous_duty = duty;
    duty = set_duty_to;
}
void Modulator::set_phase(double set_phase_to)
{
    phase = set_phase_to;
}

void Modulator::set_frequency(double set_frequency_to)
{
    Ts = 1.0/set_frequency_to;
}

double Modulator::getPWM() const {
    return deadtimecontrol.getPWM_hi();
}

double Modulator::getPWMLo() const {
    return deadtimecontrol.getPWM_lo();
}

double Modulator::get_carrier() const
{
    return carrier;
}

bool Modulator::synchronous_sample_called(double t) const
{
    return (rising_edge_when_1 != previous_edge);
}
