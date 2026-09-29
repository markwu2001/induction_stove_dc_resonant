#include "carrier_generation.hpp"

double calculate_carrier(double t, double Ts, double phase_minus_1_to_1)
{
    const double offset = Ts/0.5 + Ts*phase_minus_1_to_1;
    return std::fabs(((t + offset) / Ts - std::floor((t+ offset) / Ts )) * 2.0 - 1.0);
}

double calculate_carrier(double t, double Ts)
{
    const double offset = Ts/0.5 ;
    return std::fabs(((t + offset) / Ts - std::floor((t+ offset) / Ts )) * 2.0 - 1.0);
}
