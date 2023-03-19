/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Module:       FGMSIS.cpp
 Author:       David Culp
 Date started: 12/14/03
 Purpose:      Models the MSIS-00 atmosphere

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

FUNCTIONAL DESCRIPTION
--------------------------------------------------------------------------------
Models the NRLMSIS 2.0 atmosphere. Provides temperature and density to FGAtmosphere,
given day-of-year, time-of-day, altitude, latitude, and longitude.

HISTORY
--------------------------------------------------------------------------------
12/14/03   DPC   Created
01/11/04   DPC   Derived from FGAtmosphere
03/18/23   BC    Refactoring: update to NRLMSIS 2.0 and use F90 original code

 --------------------------------------------------------------------
 ---------  N R L M S I S E  2. 0    M O D E L    2 0 2 0  ----------
 --------------------------------------------------------------------

    The NRLMSISE 2.0 model was developed by Mike Picone, Alan Hedin, and
    Doug Drob. They also wrote a NRLMSISE-00 distribution package in
    FORTRAN which is available at
    https://ccmc.gsfc.nasa.gov/models/NRLMSIS~v2.0/

    Dominik Brodowski implemented and maintains this C version. You can
    reach him at devel@brodo.de. See the file "DOCUMENTATION" for details,
    and check http://www.brodo.de/english/pub/nrlmsise/index.html for
    updated releases of this package.
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "FGFDMExec.h"
#include "FGMSIS.h"

using namespace std;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INTERFACE WITH HTE FORTRAN CODE
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "FORTRAN_MSIS.h"

extern "C" {
  void msis_init_msisinit(const char* parmpath, const char* parmfile, int* iun,
    bool* switch_gfn, float* switch_legacy, bool* lzalt_type, bool* lspec_select,
    bool* lmass_include, bool* lN2_msis00);

  void msis_calc_msiscalc(double* day, double* utsec, double* z, double* lat,
    double* lon, const double* sfluxavg, const double* sflux, const double* ap,
    double* tn, double*  dn, double* tex);
}

namespace JSBSim {

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


MSIS::MSIS(FGFDMExec* fdmex) : FGStandardAtmosphere(fdmex)
{
  Name = "MSIS";

  Debug(0);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

MSIS::~MSIS()
{
  Debug(1);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool MSIS::InitModel(void)
{
  FGStandardAtmosphere::InitModel();

  day_of_year = 1.0;
  seconds_in_day = 0.0;

  msis_init_msisinit(nullptr, "msis20.parm", nullptr, nullptr, nullptr, nullptr,
                      nullptr, nullptr, nullptr);

  return true;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool MSIS::Load(Element* el)
{
  if (!Upload(el, true)) return false;

  if (el->FindElement("day"))
    day_of_year = el->FindElementValueAsNumber("day");
  if (el->FindElement("utc"))
    seconds_in_day = el->FindElementValueAsNumber("utc");

  return true;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void MSIS::Calculate(double altitude)
{
  double SLRair = 0.0;

  Compute(0.0, SLpressure, SLtemperature, SLdensity, SLRair);
  Compute(altitude, Pressure, Temperature, Density, Reng);

  Soundspeed  = sqrt(SHRatio*Reng*Temperature);
  SLsoundspeed  = sqrt(SHRatio*SLRair*SLtemperature);
  PressureAltitude = CalculatePressureAltitude(Pressure, altitude);
  DensityAltitude = CalculateDensityAltitude(Density, altitude);

  Viscosity = Beta * pow(Temperature, 1.5) / (SutherlandConstant + Temperature);
  KinematicViscosity = Viscosity / Density;
  // SaturatedVaporPressure = CalculateVaporPressure(Temperature);
  // ValidateVaporMassFraction(altitude);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void MSIS::Compute(double altitude, double& pressure, double& temperature,
                    double& density, double &Rair) const
{
  constexpr double fttokm = fttom / 1000.;
  constexpr double kgm3_to_slugft3 = kgtoslug / m3toft3;
  constexpr double gtoslug = kgtoslug / 1000.;
  // Molecular weight (g/mol)
  // N2 O2 O He H Ar N OA
  const double species_mmol[8] {28.013, 31.999, 15.999, 4.003, 1.008, 39.948,
                                14.007, 15.999};

  double dn[10] {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  double tex = 1.0;
  double h = altitude*fttokm;
  double lat = in.GeodLatitudeDeg;
  double lon = in.LongitudeDeg;
  double t_K = 1.0;

  // Compute epoch
  double utc_seconds = seconds_in_day + FDMExec->GetSimTime();
  unsigned int days = utc_seconds / 86400.;
  utc_seconds -= days * 86400.;
  double today = day_of_year + days;
  unsigned int year = today / 365.;
  today -= year * 365.;

  msis_calc_msiscalc(&today, &utc_seconds, &h, &lat, &lon, &f107a, &f107, ap, &t_K, dn, &tex);

  temperature = KelvinToRankine(t_K);

  // Compute specific gas constant for air
  double mmol = 0.0;
  double qty_mol = 0.0;

  for (unsigned int i=1; i<9; ++i) {
    mmol += dn[i]*species_mmol[i-1];
    qty_mol += dn[i];
  }
  double mair = mmol * gtoslug / qty_mol;
  Rair = Rstar / mair;

  density = dn[0] * kgm3_to_slugft3;
  pressure = Density * Reng * Temperature;
}

} // namespace JSBSim

