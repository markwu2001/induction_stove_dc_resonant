// heater_induction.cpp -- QSPICE C++ block, derived from heater_llc.cpp
//
// Half-bridge series-resonant induction driver, open loop.
// The switch/gate-drive/deadtime machinery is unchanged from heater_llc;
// only the "what frequency do we run at" part is different.
//
//   tank:  L1 = 39 uH (coil, pan loaded), Rpan = 1.84 ohm,
//          C1 = C2 = 0.5 uF split caps  ->  Cr_eff = 1.0 uF
//          fr = 25.5 kHz, Z0 = 6.2 ohm, Q = 3.4
//
//   run ABOVE resonance (fs > fr) so the tank current lags the bridge
//   voltage -> inductive turn-off -> ZVS.  Power goes DOWN as fs goes UP.
//
#include <cmath>
#include "../cpp_sources/modulator/modulator.hpp"
#include "../cpp_sources/carrier_generation/carrier_generation.hpp"

double
    gate_hi_voltage = 6.0    ,
    gate_lo_voltage = -3.3   ,
    dt              = 150e-9 ,   // deadtime; GaN Coss is tiny, 20 A swings the node in ns
    duty            = 0.5    ,
    vbus            = 96.0   ;   // half bridge puts +-vbus/2 on the tank

// frequency schedule (open-loop "power knob" for waveform viewing)
double fs_schedule(double t)
{
    if (t < 1.0e-3) return 28.0e3;   // ~720 W, 20 Arms, Vc ~160 Vpk, phase ~33 deg
    if (t < 2.0e-3) return 34.0e3;   // ~200 W
    return 45.0e3;                   // ~60 W
}

double Ts = 1.0/28.0e3;

Modulator modulator1(Ts, duty, gate_hi_voltage, gate_lo_voltage, dt);
Modulator modulator2(Ts, duty, gate_hi_voltage, gate_lo_voltage, dt);
DeadtimeController deadtime(gate_hi_voltage, gate_lo_voltage, dt);

double
    pwm = 0 ,
    t0  = 0 ;

union uData
{
   bool b;
   char c;
   unsigned char uc;
   short s;
   unsigned short us;
   int i;
   unsigned int ui;
   float f;
   double d;
   long long int i64;
   unsigned long long int ui64;
   char *str;
   unsigned char *bytes;
};

int __stdcall DllMain(void *module, unsigned int reason, void *reserved) { return 1; }

#undef icoil
#undef gate1
#undef gate2
#undef carrier2
#undef vin
#undef carrier1
#undef gate3
#undef gate4
#undef vout
#undef iload

extern "C" __declspec(dllexport) void heater_induction(void **opaque, double t, union uData *data)
{
   double  icoil    = data[0].d; // input  : I(L1) from Bsense -- hook for zero-cross PLL / pan detect
   double &gate1    = data[1].d; // output
   double &gate2    = data[2].d; // output
   double &carrier2 = data[3].d; // output
   double &vin      = data[4].d; // output
   double &carrier1 = data[5].d; // output
   double &gate3    = data[6].d; // output
   double &gate4    = data[7].d; // output
   double &vout     = data[8].d; // output : fs in kHz, just for plotting
   double &iload    = data[9].d; // output : unused, echoes icoil

    // half-period toggle, frequency updated every half cycle
    if (t > t0 + Ts/2) {
        t0 = t;
        Ts = 1.0/fs_schedule(t);
        pwm = (pwm == 0) ? 1 : 0;
    }

    deadtime.update(t, pwm);
    modulator1.update(t);
    modulator2.update(t);

    vin      = vbus;
    carrier1 = modulator1.get_carrier();
    carrier2 = modulator2.get_carrier();
    gate1    = deadtime.getPWM_hi();
    gate2    = deadtime.getPWM_lo();
    gate3    = gate_lo_voltage;   // no secondary side any more
    gate4    = gate_lo_voltage;
    vout     = 1.0e-3/Ts;         // kHz
    iload    = icoil;

    // TODO (next step): replace fs_schedule() with a zero-cross phase-lock on icoil,
    // and add pan detection: with no pan, Rpan -> ~0.05 ohm and L rises ~30-50 %,
    // Q blows up and the current runs away -- the controller must refuse to run.
}
