#ifndef VOLTAGECONTROLLER_H
#define VOLTAGECONTROLLER_H

class VoltageController {
public:
    VoltageController(double vkp, double vki, double upper_limit, double lower_limit);

    double compute(double v_error);

private:
    double vkp;
    double vki;
    double upper_limit;
    double lower_limit;
    double i_term;

    double sgn(double input);
};

#endif // VOLTAGECONTROLLER_H

