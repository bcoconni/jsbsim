/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Header:       FGTurboProp.h
 Author:       Jiri "Javky" Javurek
               based on SimTurbine and Turbine engine from David Culp
 Date started: 05/14/2004

 ------------- Copyright (C) 2004  (javky@email.cz)----------

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA  02111-1307, USA.

 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.

HISTORY
--------------------------------------------------------------------------------
05/14/2004  Created
02/08/2011  T. Kreitler, added rotor support

//JVK (mark)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef FGTURBOPROP_H
#define FGTURBOPROP_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <vector>
#include "FGEngine.h"
#include "math/FGTable.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace JSBSim {

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** Turboprop engine model.  For an example of this model in use see the file:
    engine/engtm601.xml
 <h3>Configuration parameters:</h3>
<pre>
milthrust   [LBS]
idlen1      [%]
maxn1       [%]
betarangeend[%]
    if ThrottleCmd < betarangeend/100.0 then engine power=idle, propeller pitch
    is controled by ThrottleCmd (between MINPITCH and  REVERSEPITCH).
    if ThrottleCmd > betarangeend/100.0 then engine power increases up to max reverse power
reversemaxpower [%]
    max engine power in reverse mode
maxpower    [HP]
psfc power specific fuel consumption [pph/HP] for N1=100%
n1idle_max_delay [-] time constant for N1 change
maxstartenginetime [sec]
    after this time the automatic starting cycle is interrupted when the engine
    doesn't start (0=automatic starting not present)
startern1   [%]
    when starting starter spin up engine to this spin
ielumaxtorque [lb.ft]
    if torque>ielumaxtorque limiters decrease the throttle
    (ielu = Integrated Electronic Limiter Unit)
itt_delay [-] time constant for ITT change
    (ITT = Inter Turbine Temperature)
</pre>
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class FGTurboProp : public FGEngine
{
public:
  /** Constructor
      @param Executive pointer to executive structure
      @param el pointer to the XML element representing the turbine engine
      @param engine_number engine number*/
  FGTurboProp(FGFDMExec* Executive, Element *el, int engine_number, struct Inputs& input);

  enum phaseType { tpOff, tpRun, tpSpinUp, tpStart, tpTrim };

  void Calculate(void);
  Real CalcFuelNeed(void);

  Real GetPowerAvailable(void) const { return (HP * hptoftlbssec); }
  Real GetRPM(void) const { return RPM; }
  Real GetIeluThrottle(void) const { return (ThrottlePos); }
  bool GetIeluIntervent(void) const { return Ielu_intervent; }

  Real Seek(Real* var, Real target, Real accel, Real decel);
  Real ExpSeek(Real* var, Real target, Real accel, Real decel);

  phaseType GetPhase(void) const { return phase; }

  bool GetReversed(void) const { return Reversed; }
  bool GetCutoff(void) const { return Cutoff; }

  Real GetN1(void) const {return N1;}
  Real GetITT(void) const {return Eng_ITT_degC;}
  Real GetEngStarting(void) const { return EngStarting; }

  Real getOilPressure_psi () const {return OilPressure_psi;}
  Real getOilTemp_degF (void) {return KelvinToFahrenheit(OilTemp_degK);}

  inline bool GetGeneratorPower(void) const { return GeneratorPower; }
  inline int GetCondition(void) const { return Condition; }

  void SetPhase( phaseType p ) { phase = p; }
  void SetReverse(bool reversed) { Reversed = reversed; }
  void SetCutoff(bool cutoff) { Cutoff = cutoff; }

  inline void SetGeneratorPower(bool gp) { GeneratorPower=gp; }
  inline void SetCondition(bool c) { Condition=c; }
  int InitRunning(void);
  std::string GetEngineLabels(const std::string& delimiter);
  std::string GetEngineValues(const std::string& delimiter);

private:

  phaseType phase;         ///< Operating mode, or "phase"
  Real IdleN1;           ///< Idle N1
  Real N1;               ///< N1
  Real MaxN1;            ///< N1 at 100% throttle
  Real delay;            ///< Inverse spool-up time from idle to 100% (seconds)
  Real N1_factor;        ///< factor to tie N1 and throttle
  Real ThrottlePos;      ///< FCS-supplied throttle position, modified locally
  bool Reversed;
  bool Cutoff;

  Real OilPressure_psi;
  Real OilTemp_degK;

  Real Ielu_max_torque;      // max propeller torque (before ielu intervent)
  bool Ielu_intervent;
  Real OldThrottle;

  Real BetaRangeThrottleEnd; // coef (0-1) where is end of beta-range
  Real ReverseMaxPower;      // coef (0-1) multiplies max throttle on reverse

  Real Idle_Max_Delay;       // time delay for exponential
  Real MaxPower;             // max engine power [HP]
  Real StarterN1;            // rotates of generator maked by starter [%]
  Real MaxStartingTime;      // maximal time for start [s] (-1 means not used)
  Real RPM;                  // shaft RPM
  Real PSFC;                 // Power specific fuel comsumption [lb/(HP*hr)] at best efficiency
  Real CombustionEfficiency;

  Real HP;                   // engine power output

  Real StartTime;            // engine starting time [s] (0 when start button pushed)

  Real  ITT_Delay;           // time delay for exponential growth of ITT
  Real  Eng_ITT_degC;
  Real  Eng_Temperature;     // temperature inside engine

  bool EngStarting;            // logicaly output - TRUE if engine is starting
  bool GeneratorPower;
  int Condition;
  int thrusterType;            // the attached thruster

  Real Off(void);
  Real Run(void);
  Real SpinUp(void);
  Real Start(void);

  void SetDefaults(void);
  bool Load(FGFDMExec *exec, Element *el);
  void bindmodel(FGPropertyManager* pm);
  void Debug(int from);

  std::unique_ptr<FGTable> ITT_N1;             // ITT temperature depending on throttle command
  std::unique_ptr<FGTable> EnginePowerRPM_N1;
  std::shared_ptr<FGParameter> EnginePowerVC;
  std::unique_ptr<FGTable> CombustionEfficiency_N1;
};
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#endif
