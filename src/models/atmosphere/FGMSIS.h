/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Header:       FGMSIS.h
 Description:  NRLMSIS 2.0 Atmosphere
 Author:       David Culp
 Date started: 12/14/03

 ------------- Copyright (C) 2003  David P. Culp (davidculp2@comcast.net) ------

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
12/14/03   DPC   Created
01/11/04   DPC   Derive from FGAtmosphere
03/18/23   BC    Refactoring: update to NRLMSIS 2.0 and use F90 original code

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef FGMSIS_H
#define FGMSIS_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "models/atmosphere/FGStandardAtmosphere.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef USE_FORTRAN_MSIS
extern "C" {
#include "MSIS/nrlmsise-00.h"
}
#endif

namespace JSBSim {

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** Models the NRLMSIS 2.0 atmosphere.
    This is an interface for the NRLMSIS 2.0:

    The NRLMSISE 2.0 model was developed by Mike Picone, Alan Hedin, and
    Doug Drob. They also wrote a NRLMSISE-00 distribution package in
    FORTRAN which is available at
    https://ccmc.gsfc.nasa.gov/models/NRLMSIS~v2.0/

    Dominik Brodowski implemented and maintains this C version. You can
    reach him at devel@brodo.de. See the file "DOCUMENTATION" for details,
    and check http://www.brodo.de/english/pub/nrlmsise/index.html for
    updated releases of this package.
    @author David Culp
*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class JSBSIM_API MSIS : public FGStandardAtmosphere
{
public:

  /// Constructor
  MSIS(FGFDMExec*);
  /// Destructor
  ~MSIS();
  /** Runs the NRLMSIS 2.0 atmosphere model; called by the Executive
      Can pass in a value indicating if the executive is directing the simulation to Hold.
      @param Holding if true, the executive has been directed to hold the sim from
                     advancing time. Some models may ignore this flag, such as the Input
                     model, which may need to be active to listen on a socket for the
                     "Resume" command to be given.
      @return false if no error */

  bool InitModel(void) override;
  bool Load(Element* el) override;

  using FGAtmosphere::GetTemperature;  // Prevent C++ from hiding GetTemperature(void)
  double GetTemperature(double altitude) const override {
    double t, p, rho, R;
    Compute(altitude, p, t, rho, R);
    return t;
  }

  using FGAtmosphere::GetPressure;  // Prevent C++ from hiding GetPressure(void)
  double GetPressure(double altitude) const override {
    double t, p, rho, R;
    Compute(altitude, p, t, rho, R);
    return p;
  }

  using FGAtmosphere::GetDensity;  // Prevent C++ from hiding GetDensity(void)
  double GetDensity(double altitude) const override {
    double t, p, rho, R;
    Compute(altitude, p, t, rho, R);
    return rho;
  }

  using FGAtmosphere::GetSoundSpeed;  // Prevent C++ from hiding GetSoundSpeed(void)
  double GetSoundSpeed(double altitude) const override {
    double t, p, rho, R;
    Compute(altitude, p, t, rho, R);
    return sqrt(FGAtmosphere::SHRatio*R*t);
  }

protected:
  void Calculate(double altitude) override;
  void Compute(double altitude, double& pression, double& temperature,
                double& density, double &Rair) const;

  double day_of_year = 1.0;
  double seconds_in_day = 0.0;
  double f107a = 150.0;
  double f107 = 150.0;
  double ap[7] {4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

#ifndef USE_FORTRAN_MSIS
  mutable struct nrlmsise_flags flags;
  mutable struct nrlmsise_input input;
#endif

private:
  // Setting temperature & pressure is not allowed in this model.
  void SetTemperature(double t, double h, eTemperature unit) override {};
  void SetTemperatureSL(double t, eTemperature unit) override {};
  void SetPressureSL(ePressure unit, double pressure) override {};
};

} // namespace JSBSim

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#endif
