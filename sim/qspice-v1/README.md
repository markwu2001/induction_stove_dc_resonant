# heater_induction — QSPICE model of a half-bridge induction heater

Derived from [`heater_llc`](https://github.com/johonkanen/qspice_dab_and_pfc/tree/main/qspice_ref_models/heater_llc)
in johonkanen/qspice_dab_and_pfc.

The original LLC schematic is already a split-capacitor half-bridge series-resonant
stage: C1/C2 in series with L1 is the induction-cooker tank. So this model keeps the
primary side, gate drives, and deadtime logic unchanged. It removes everything
downstream of `middle_point` and replaces it with a pan model.

## Files

| Path | Contents |
|---|---|
| `heater_induction.qsch` | Top-level schematic |
| `heater_induction/heater_induction.qsch` | Same circuit (differs only in `icoil` wire routing); lives next to the DLL |
| `heater_induction/heater_induction.cpp` | C++ block (was `heater_llc.cpp`) |
| `heater_induction/build_instructions.md` | DMC build command |
| `cpp_sources/` | Shared modulator, carrier, deadtime, and control sources from the original repo |

## Schematic changes

**Removed (secondary side):**
- X1 transformer
- L2 (350 µH magnetizing inductance)
- Output rectifier D2, D4
- Output cap C5
- Load current source
- Right-hand `vdc` rail

A GND symbol goes back on the bottom rail.

**Changed tank values:**

| Part | Original | Now | Why |
|---|---|---|---|
| L1 | 42 µH | 39 µH | ~13-turn rewound coil, pan loaded |
| C1, C2 | 10 nF each | 0.5 µF each | The 2× 0.5 µF / 630 V polypropylene caps |
| Rpan | — | 1.84 Ω | New; placed where L2 was, between `afterL1` and `middle_point` |

- **Rpan is the pan.** It models the coil plus pan reflected as a series L-R. Pan power is `I(Rpan)² · 1.84`, which you can probe directly.
- **Effective resonant capacitance.** From the tank's view, C1 and C2 are in parallel (each goes from `middle_point` to a DC rail), so:
  - Cr_eff = 1.0 µF
  - fr ≈ 25.5 kHz, Z₀ ≈ 6.2 Ω, Q ≈ 3.4

**Sense and directives:**
- The block's `vdc` input had nothing left to sense. It is now `icoil`, fed from a B-source `Bsense: V=I(L1)`. It isn't used yet; it's the hook for zero-cross phase-lock and pan detection.
- `.ic V(vdc)=0` → `.ic V(middle_point)=48` (= vbus/2).
- `.tran 0 3e-3 0 20n`: 3 ms is plenty, since the tank time constant 2L/R ≈ 42 µs.

## C++ changes (`heater_llc.cpp` → `heater_induction.cpp`)

- DLL and exported function renamed to `heater_induction`.
- Output-voltage PI loop and load stepping removed.
- Ts now comes from an open-loop `fs_schedule()`. As frequency rises, power falls:

  | Time | fs | Approx. power |
  |---|---|---|
  | 0–1 ms | 28 kHz | ~720 W (20 A rms, ~33° lag) |
  | 1–2 ms | 34 kHz | ~200 W |
  | 2–3 ms | 45 kHz | ~60 W |

- `vbus = 96`. A half bridge puts only ±vbus/2 on the tank, so 96 V here matches a 48 V full bridge. Set it to 48 to see what the pack alone would give (~10 A rms, ~180 W at 28 kHz).
- `vout` now outputs fs in kHz for plotting. `iload` echoes `icoil`.
- `gate3` and `gate4` (secondary-side drives) are held low.
- Deadtime is 150 ns (`dt`).

## Build

Recompile the DLL after any C++ change. From `heater_induction/`, in PowerShell:

```powershell
& "D:\Program Files\QSPICE\dm\bin\dmc.exe" -mn -WD heater_induction.cpp ..\cpp_sources\modulator\modulator.cpp ..\cpp_sources\carrier_generation\carrier_generation.cpp ..\cpp_sources\deadtime\deadtimecontroller.cpp kernel32.lib
```

`deadtimecontroller.cpp` is required. Leaving it out causes `Error 42: Symbol Undefined DeadtimeController...` at link time, and no new DLL is written.

## What to look at

- **V(middle_point)** (resonant cap voltage) swings to about ±160 V peak around 48 V at 28 kHz, well beyond the bus rails. This is the series-resonant Q, and it's why the caps are rated 630 V.
- **I(L1)** is a near-sinusoid of about 28 A peak. At each gate transition it should flow in the direction that commutates `bridge_voltage` before the opposite device turns on (ZVS). The drive runs above resonance, so the current lags the voltage.
- **Pan removed:** set Rpan = 0.05 Ω and L1 = 55 µH. Q rises sharply and the current runs away. Pan detection exists to catch this case.

## Next steps

- Replace `fs_schedule()` with a zero-cross phase-lock on `icoil`.
- Add pan detection: refuse to run when the tank looks unloaded.
