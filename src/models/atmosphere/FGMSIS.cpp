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
03/18/23   BC    Refactored
07/21/23   BC    Update to NRLMSIS 2.0 and use F90 original code

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
INTERFACE WITH THE FORTRAN CODE
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifdef USE_FORTRAN_MSIS
#include "FORTRAN_MSIS.h"

extern "C" {
  void init(const char* parmpath, const char* parmfile, bool* filefound);

  void msis_calc_msiscalc(double* day, double* utsec, double* z, double* lat,
    double* lon, const double* sfluxavg, const double* sflux, const double* ap,
    double* tn, double*  dn, double* tex);
}
#endif

namespace JSBSim {

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


FGMSIS::FGMSIS(FGFDMExec* fdmex) : FGStandardAtmosphere(fdmex)
{
  Name = "MSIS";

#ifdef USE_FORTRAN_MSIS
  bool filefound = true;
  SGPath datapath = fdmex->GetDataPath();
  string filepath = "msis20.parm";

  if (!datapath.isNull())
    filepath = (datapath/filepath).str();

  const char* filename = filepath.c_str();

  init(nullptr, filename, &filefound);
  if (!filefound)
    throw BaseException("Could not find " + filepath);
#else
  for(unsigned int i=0; i<24; ++i)
    flags.switches[i] = 1;
  input.year = 0;  // Ignored by NRLMSIS
  input.f107A = f107a;
  input.f107 = f107;
  input.ap = ap[0];
  input.ap_a = nullptr;
#endif

  Debug(0);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGMSIS::~FGMSIS()
{
  Debug(1);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool FGMSIS::InitModel(void)
{
  FGStandardAtmosphere::InitModel();

  Calculate(0.0);

  return true;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool FGMSIS::Load(Element* el)
{
  if (!Upload(el, true)) return false;

  if (el->FindElement("day"))
    day_of_year = el->FindElementValueAsNumber("day");
  if (el->FindElement("utc"))
    seconds_in_day = el->FindElementValueAsNumber("utc");

  Debug(3);

  return true;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void FGMSIS::Calculate(double altitude)
{
  double SLRair = 0.0;

  Compute(0.0, SLpressure, SLtemperature, SLdensity, SLRair);
  Compute(altitude, Pressure, Temperature, Density, Reng);

  SLsoundspeed  = sqrt(SHRatio*SLRair*SLtemperature);
  Soundspeed  = sqrt(SHRatio*Reng*Temperature);
  PressureAltitude = CalculatePressureAltitude(Pressure, altitude);
  DensityAltitude = CalculateDensityAltitude(Density, altitude);

  Viscosity = Beta * pow(Temperature, 1.5) / (SutherlandConstant + Temperature);
  KinematicViscosity = Viscosity / Density;
  // SaturatedVaporPressure = CalculateVaporPressure(Temperature);
  // ValidateVaporMassFraction(altitude);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void FGMSIS::Compute(double altitude, double& pressure, double& temperature,
                    double& density, double &Rair) const
{
  constexpr double fttokm = fttom / 1000.;
  constexpr double kgm3_to_slugft3 = kgtoslug / m3toft3;
  constexpr double gtoslug = kgtoslug / 1000.;
  // Molecular weight (g/mol)
  // N2 O2 O He H Ar N OA
  const double species_mmol[8] {28.0134, 31.9988, 31.9988/2.0, 4.0, 1.0, 39.948,
                                28.0134/2.0, 31.9988/2.0};

  double dn[10] {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  double h = altitude*fttokm;
  double lat = in.GeodLatitudeDeg;
  double lon = in.LongitudeDeg;

  // Compute epoch
  double utc_seconds = seconds_in_day + FDMExec->GetSimTime();
  unsigned int days = utc_seconds / 86400.;
  utc_seconds -= days * 86400.;
  double today = day_of_year + days;
  unsigned int year = today / 365.;
  today -= year * 365.;

#ifdef USE_FORTRAN_MSIS
  double t_K = 1.0;
  double tex = 1.0;

  msis_calc_msiscalc(&today, &utc_seconds, &h, &lat, &lon, &f107a, &f107, ap, &t_K, dn, &tex);

  temperature = KelvinToRankine(t_K);
  density = dn[0] * kgm3_to_slugft3;
#else
  struct nrlmsise_output output;

  input.doy = today;
  input.sec = utc_seconds;
  input.alt = h;
  input.g_lat = lat;
  input.g_long = lon;
  input.lst = utc_seconds/3600 + lon/15;  // Local Solar Time (hours)
  assert(flags.switches[9] != -1);        // Make sure that input.ap is used.

  gtd7(&input, &flags, &output);

  temperature = KelvinToRankine(output.t[1]);
  density = output.d[5] * kgm3_to_slugft3;
  dn[1] = output.d[2];  // N2
  dn[2] = output.d[3];  // O2
  dn[3] = output.d[1];  // O
  dn[4] = output.d[0];  // He
  dn[5] = output.d[6];  // H
  dn[6] = output.d[4];  // Ar
  dn[7] = output.d[7];  // N
  // SUBROUTINE GTD7 does NOT include anomalous oxygen so we drop it from
  // the molar mass computation as well for consistency.
  dn[8] = 0.0;          // OA
#endif

  // Compute specific gas constant for air
  double mmol = 0.0;
  double qty_mol = 0.0;

  for (unsigned int i=1; i<9; ++i) {
    mmol += dn[i]*species_mmol[i-1];
    qty_mol += dn[i];
  }
  double mair = mmol * gtoslug / qty_mol;
  Rair = Rstar / mair;

  pressure = density * Rair * temperature;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//    The bitmasked value choices are as follows:
//    unset: In this case (the default) JSBSim would only print
//       out the normally expected messages, essentially echoing
//       the config files as they are read. If the environment
//       variable is not set, debug_lvl is set to 1 internally
//    0: This requests JSBSim not to output any messages
//       whatsoever.
//    1: This value explicity requests the normal JSBSim
//       startup messages
//    2: This value asks for a message to be printed out when
//       a class is instantiated
//    4: When this value is set, a message is displayed when a
//       FGModel object executes its Run() method
//    8: When this value is set, various runtime state variables
//       are printed out periodically
//    16: When set various parameters are sanity checked and
//       a message is printed out when they go out of bounds

void FGMSIS::Debug(int from)
{
  if (debug_lvl <= 0) return;

  if (debug_lvl & 1) { // Standard console startup message output
    if (from == 0) {} // Constructor
    if (from == 3) { // Loading
      cout << "    NRLMSIS atmosphere model" << endl;
      cout << "      day: " << day_of_year << endl;
      cout << "      UTC: " << seconds_in_day << endl << endl;
    }
  }
  if (debug_lvl & 2 ) { // Instantiation/Destruction notification
    if (from == 0) std::cout << "Instantiated: MSIS" << std::endl;
    if (from == 1) std::cout << "Destroyed:    MSIS" << std::endl;
  }
  if (debug_lvl & 4 ) { // Run() method entry print for FGModel-derived objects
  }
  if (debug_lvl & 8 ) { // Runtime state variables
  }
  if (debug_lvl & 16) { // Sanity checking
  }
  if (debug_lvl & 128) { //
  }
  if (debug_lvl & 64) {
    if (from == 0) { // Constructor
    }
  }
}

} // namespace JSBSim
