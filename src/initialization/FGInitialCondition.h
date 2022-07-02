/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Header:       FGInitialCondition.h
 Author:       Tony Peden
 Date started: 7/1/99

 --------- Copyright (C) 1999  Anthony K. Peden (apeden@earthlink.net) ---------

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; either version 2 of the License, or (at your option) any
 later version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along
 with this program; if not, write to the Free Software Foundation, Inc., 59
 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

 Further information about the GNU Lesser General Public License can also be
 found on the world wide web at http://www.gnu.org.

 HISTORY
--------------------------------------------------------------------------------
7/1/99   TP   Created

FUNCTIONAL DESCRIPTION
--------------------------------------------------------------------------------

The purpose of this class is to take a set of initial conditions and provide a
kinematically consistent set of body axis velocity components, euler angles, and
altitude. This class does not attempt to trim the model i.e. the sim will most
likely start in a very dynamic state (unless, of course, you have chosen your
IC's wisely) even after setting it up with this class.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef FGINITIALCONDITION_H
#define FGINITIALCONDITION_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <memory>

#include "math/FGLocation.h"
#include "math/FGQuaternion.h"
#include "simgear/misc/sg_path.hxx"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace JSBSim {

class FGFDMExec;
class FGMatrix33;
class FGColumnVector3;
class FGAtmosphere;
class FGAircraft;
class FGPropertyManager;
class Element;

typedef enum { setvt, setvc, setve, setmach, setuvw, setned, setvg } speedset;
typedef enum { setasl, setagl } altitudeset;
typedef enum { setgeoc, setgeod } latitudeset;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** Initializes the simulation run.
    Takes a set of initial conditions (IC) and provide a kinematically
    consistent set of body axis velocity components, euler angles, and altitude.
    This class does not attempt to trim the model i.e. the sim will most likely
    start in a very dynamic state (unless, of course, you have chosen your IC's
    wisely, or started on the ground) even after setting it up with this class.

   <h3>Usage Notes</h3>

   With a valid object of FGFDMExec and an aircraft model loaded:

   @code
   FGInitialCondition* fgic = FDMExec->GetIC();

   // Reset the initial conditions and set VCAS and altitude
   fgic->InitializeIC();
   fgic->SetVcalibratedKtsIC(vcas);
   fgic->SetAltitudeAGLFtIC(altitude);

   // directly into Run
   FDMExec->GetPropagate()->SetInitialState(fgic);
   delete fgic;
   FDMExec->Run();

   //or to loop the sim w/o integrating
   FDMExec->RunIC();
   @endcode

   Alternatively, you can load initial conditions from an XML file:

   @code
   FGInitialCondition* fgic = FDMExec->GetIC();
   fgic->Load(IC_file);
   @endcode

   <h3>Speed</h3>

   Since vc, ve, vt, and mach all represent speed, the remaining
   three are recalculated each time one of them is set (using the
   current altitude).  The most recent speed set is remembered so
   that if and when altitude is reset, the last set speed is used
   to recalculate the remaining three. Setting any of the body
   components forces a recalculation of vt and vt then becomes the
   most recent speed set.

   <h3>Alpha,Gamma, and Theta</h3>

   This class assumes that it will be used to set up the sim for a
   steady, zero pitch rate condition. Since any two of those angles
   specifies the third gamma (flight path angle) is favored when setting
   alpha and theta and alpha is favored when setting gamma. i.e.

   - set alpha : recalculate theta using gamma as currently set
   - set theta : recalculate alpha using gamma as currently set
   - set gamma : recalculate theta using alpha as currently set

   The idea being that gamma is most interesting to pilots (since it
   is indicative of climb rate).

   Setting climb rate is, for the purpose of this discussion,
   considered equivalent to setting gamma.

   These are the items that can be set in an initialization file:

   - ubody (velocity, ft/sec)
   - vbody (velocity, ft/sec)
   - wbody (velocity, ft/sec)
   - vnorth (velocity, ft/sec)
   - veast (velocity, ft/sec)
   - vdown (velocity, ft/sec)
   - latitude (position, degrees)
   - longitude (position, degrees)
   - phi (orientation, degrees)
   - theta (orientation, degrees)
   - psi (orientation, degrees)
   - alpha (angle, degrees)
   - beta (angle, degrees)
   - gamma (angle, degrees)
   - roc (vertical velocity, ft/sec)
   - elevation (local terrain elevation, ft)
   - altitude (altitude AGL, ft)
   - altitudeAGL (altitude AGL, ft)
   - altitudeMSL (altitude MSL, ft)
   - winddir (wind from-angle, degrees)
   - vwind (magnitude wind speed, ft/sec)
   - hwind (headwind speed, knots)
   - xwind (crosswind speed, knots)
   - vc (calibrated airspeed, ft/sec)
   - mach (mach)
   - vground (ground speed, ft/sec)
   - trim (0 for no trim, 1 for ground trim, 'Longitudinal', 'Full', 'Ground', 'Pullup', 'Custom', 'Turn')
   - running (-1 for all engines, 0 ... n-1 for specific engines)

   <h3>Properties</h3>
   @property ic/vc-kts (read/write) Calibrated airspeed initial condition in knots
   @property ic/ve-kts (read/write) Knots equivalent airspeed initial condition
   @property ic/vg-kts (read/write) Ground speed initial condition in knots
   @property ic/vt-kts (read/write) True airspeed initial condition in knots
   @property ic/mach (read/write) Mach initial condition
   @property ic/roc-fpm (read/write) Rate of climb initial condition in feet/minute
   @property ic/gamma-deg (read/write) Flightpath angle initial condition in degrees
   @property ic/alpha-deg (read/write) Angle of attack initial condition in degrees
   @property ic/beta-deg (read/write) Angle of sideslip initial condition in degrees
   @property ic/theta-deg (read/write) Pitch angle initial condition in degrees
   @property ic/phi-deg (read/write) Roll angle initial condition in degrees
   @property ic/psi-true-deg (read/write) Heading angle initial condition in degrees
   @property ic/lat-gc-deg (read/write) Latitude initial condition in degrees
   @property ic/long-gc-deg (read/write) Longitude initial condition in degrees
   @property ic/h-sl-ft (read/write) Height above sea level initial condition in feet
   @property ic/h-agl-ft (read/write) Height above ground level initial condition in feet
   @property ic/sea-level-radius-ft (read/write) Radius of planet at sea level in feet
   @property ic/terrain-elevation-ft (read/write) Terrain elevation above sea level in feet
   @property ic/vg-fps (read/write) Ground speed initial condition in feet/second
   @property ic/vt-fps (read/write) True airspeed initial condition in feet/second
   @property ic/vw-bx-fps (read/write) Wind velocity initial condition in Body X frame in feet/second
   @property ic/vw-by-fps (read/write) Wind velocity initial condition in Body Y frame in feet/second
   @property ic/vw-bz-fps (read/write) Wind velocity initial condition in Body Z frame in feet/second
   @property ic/vw-north-fps (read/write) Wind northward velocity initial condition in feet/second
   @property ic/vw-east-fps (read/write) Wind eastward velocity initial condition in feet/second
   @property ic/vw-down-fps (read/write) Wind downward velocity initial condition in feet/second
   @property ic/vw-mag-fps (read/write) Wind velocity magnitude initial condition in feet/sec.
   @property ic/vw-dir-deg (read/write) Wind direction initial condition, in degrees from north
   @property ic/roc-fps (read/write) Rate of climb initial condition, in feet/second
   @property ic/u-fps (read/write) Body frame x-axis velocity initial condition in feet/second
   @property ic/v-fps (read/write) Body frame y-axis velocity initial condition in feet/second
   @property ic/w-fps (read/write) Body frame z-axis velocity initial condition in feet/second
   @property ic/vn-fps (read/write) Local frame x-axis (north) velocity initial condition in feet/second
   @property ic/ve-fps (read/write) Local frame y-axis (east) velocity initial condition in feet/second
   @property ic/vd-fps (read/write) Local frame z-axis (down) velocity initial condition in feet/second
   @property ic/gamma-rad (read/write) Flight path angle initial condition in radians
   @property ic/alpha-rad (read/write) Angle of attack initial condition in radians
   @property ic/theta-rad (read/write) Pitch angle initial condition in radians
   @property ic/beta-rad (read/write) Angle of sideslip initial condition in radians
   @property ic/phi-rad (read/write) Roll angle initial condition in radians
   @property ic/psi-true-rad (read/write) Heading angle initial condition in radians
   @property ic/lat-gc-rad (read/write) Geocentric latitude initial condition in radians
   @property ic/long-gc-rad (read/write) Longitude initial condition in radians
   @property ic/p-rad_sec (read/write) Roll rate initial condition in radians/second
   @property ic/q-rad_sec (read/write) Pitch rate initial condition in radians/second
   @property ic/r-rad_sec (read/write) Yaw rate initial condition in radians/second

   @author Tony Peden
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class JSBSIM_API FGInitialCondition : public FGJSBBase
{
public:
  /// Constructor
  explicit FGInitialCondition(FGFDMExec *fdmex);
  /// Destructor
  ~FGInitialCondition();

  /** Set calibrated airspeed initial condition in knots.
      @param vc Calibrated airspeed in knots  */
  void SetVcalibratedKtsIC(Real vc);

  /** Set equivalent airspeed initial condition in knots.
      @param ve Equivalent airspeed in knots  */
  void SetVequivalentKtsIC(Real ve);

  /** Set true airspeed initial condition in knots.
      @param vtrue True airspeed in knots  */
  void SetVtrueKtsIC(Real vtrue) { SetVtrueFpsIC(vtrue*ktstofps); }

  /** Set ground speed initial condition in knots.
      @param vg Ground speed in knots  */
  void SetVgroundKtsIC(Real vg) { SetVgroundFpsIC(vg*ktstofps); }

  /** Set mach initial condition.
      @param mach Mach number  */
  void SetMachIC(Real mach);

  /** Sets angle of attack initial condition in degrees.
      @param a Alpha in degrees */
  void SetAlphaDegIC(Real a) { SetAlphaRadIC(a*degtorad); }

  /** Sets angle of sideslip initial condition in degrees.
      @param B Beta in degrees */
  void SetBetaDegIC(Real b) { SetBetaRadIC(b*degtorad);}

  /** Sets pitch angle initial condition in degrees.
      @param theta Theta (pitch) angle in degrees */
  void SetThetaDegIC(Real theta) { SetThetaRadIC(theta*degtorad); }

  /** Resets the IC data structure to new values
      @param u, v, w, ... **/
  void ResetIC(Real u0, Real v0, Real w0, Real p0, Real q0, Real r0,
               Real alpha0, Real beta0, Real phi0, Real theta0, Real psi0,
               Real latitudeRad0, Real longitudeRad0, Real altitudeAGL0,
               Real gamma0);

  /** Sets the roll angle initial condition in degrees.
      @param phi roll angle in degrees */
  void SetPhiDegIC(Real phi)  { SetPhiRadIC(phi*degtorad);}

  /** Sets the heading angle initial condition in degrees.
      @param psi Heading angle in degrees */
  void SetPsiDegIC(Real psi){ SetPsiRadIC(psi*degtorad); }

  /** Sets the climb rate initial condition in feet/minute.
      @param roc Rate of Climb in feet/minute  */
  void SetClimbRateFpmIC(Real roc) { SetClimbRateFpsIC(roc/60.0); }

  /** Sets the flight path angle initial condition in degrees.
      @param gamma Flight path angle in degrees  */
  void SetFlightPathAngleDegIC(Real gamma)
  { SetClimbRateFpsIC(vt*sin(gamma*degtorad)); }

  /** Sets the altitude above sea level initial condition in feet.
      @param altitudeASL Altitude above sea level in feet */
  void SetAltitudeASLFtIC(Real altitudeASL);

  /** Sets the initial Altitude above ground level.
      @param agl Altitude above ground level in feet */
  void SetAltitudeAGLFtIC(Real agl);

  /** Sets the initial terrain elevation.
      @param elev Initial terrain elevation in feet */
  void SetTerrainElevationFtIC(Real elev);

  /** Sets the initial latitude.
      @param lat Initial latitude in degrees */
  void SetLatitudeDegIC(Real lat) { SetLatitudeRadIC(lat*degtorad); }

  /** Sets the initial geodetic latitude.
      This method modifies the geodetic altitude in order to keep the altitude
      above sea level unchanged.
      @param glat Initial geodetic latitude in degrees */
  void SetGeodLatitudeDegIC(Real glat)
  { SetGeodLatitudeRadIC(glat*degtorad); }

  /** Sets the initial longitude.
      @param lon Initial longitude in degrees */
  void SetLongitudeDegIC(Real lon) { SetLongitudeRadIC(lon*degtorad); }

  /** Gets the initial calibrated airspeed.
      @return Initial calibrated airspeed in knots */
  Real GetVcalibratedKtsIC(void) const;

  /** Gets the initial equivalent airspeed.
      @return Initial equivalent airspeed in knots */
  Real GetVequivalentKtsIC(void) const;

  /** Gets the initial ground speed.
      @return Initial ground speed in knots */
  Real GetVgroundKtsIC(void) const { return GetVgroundFpsIC() * fpstokts; }

  /** Gets the initial true velocity.
      @return Initial true airspeed in knots. */
  Real GetVtrueKtsIC(void) const { return vt*fpstokts; }

  /** Gets the initial mach.
      @return Initial mach number */
  Real GetMachIC(void) const;

  /** Gets the initial climb rate.
      @return Initial climb rate in feet/minute */
  Real GetClimbRateFpmIC(void) const
  { return GetClimbRateFpsIC()*60; }

  /** Gets the initial flight path angle.
      @return Initial flight path angle in degrees */
  Real GetFlightPathAngleDegIC(void) const
  { return GetFlightPathAngleRadIC()*radtodeg; }

  /** Gets the initial angle of attack.
      @return Initial alpha in degrees */
  Real GetAlphaDegIC(void) const { return alpha*radtodeg; }

  /** Gets the initial sideslip angle.
      @return Initial beta in degrees */
  Real GetBetaDegIC(void) const  { return beta*radtodeg; }

  /** Gets the initial pitch angle.
      @return Initial pitch angle in degrees */
  Real GetThetaDegIC(void) const { return orientation.GetEulerDeg(eTht); }

  /** Gets the initial roll angle.
      @return Initial phi in degrees */
  Real GetPhiDegIC(void) const { return orientation.GetEulerDeg(ePhi); }

  /** Gets the initial heading angle.
      @return Initial psi in degrees */
  Real GetPsiDegIC(void) const { return orientation.GetEulerDeg(ePsi); }

  /** Gets the initial latitude.
      @return Initial geocentric latitude in degrees */
  Real GetLatitudeDegIC(void) const { return position.GetLatitudeDeg(); }

  /** Gets the initial geodetic latitude.
      @return Initial geodetic latitude in degrees */
  Real GetGeodLatitudeDegIC(void) const
  { return position.GetGeodLatitudeDeg(); }

  /** Gets the initial longitude.
      @return Initial longitude in degrees */
  Real GetLongitudeDegIC(void) const { return position.GetLongitudeDeg(); }

  /** Gets the initial altitude above sea level.
      @return Initial altitude in feet. */
  Real GetAltitudeASLFtIC(void) const;

  /** Gets the initial altitude above ground level.
      @return Initial altitude AGL in feet */
  Real GetAltitudeAGLFtIC(void) const;

  /** Gets the initial terrain elevation.
      @return Initial terrain elevation in feet */
  Real GetTerrainElevationFtIC(void) const;

  /** Gets the initial Earth position angle.
      @return Initial Earth position angle in radians. */
  Real GetEarthPositionAngleIC(void) const { return epa; }

  /** Sets the initial ground speed.
      @param vg Initial ground speed in feet/second */
  void SetVgroundFpsIC(Real vg);

  /** Sets the initial true airspeed.
      @param vt Initial true airspeed in feet/second */
  void SetVtrueFpsIC(Real vt);

  /** Sets the initial body axis X velocity.
      @param ubody Initial X velocity in feet/second */
  void SetUBodyFpsIC(Real ubody) { SetBodyVelFpsIC(eU, ubody); }

  /** Sets the initial body axis Y velocity.
      @param vbody Initial Y velocity in feet/second */
  void SetVBodyFpsIC(Real vbody) { SetBodyVelFpsIC(eV, vbody); }

  /** Sets the initial body axis Z velocity.
      @param wbody Initial Z velocity in feet/second */
  void SetWBodyFpsIC(Real wbody) { SetBodyVelFpsIC(eW, wbody); }

  /** Sets the initial local axis north velocity.
      @param vn Initial north velocity in feet/second */
  void SetVNorthFpsIC(Real vn) { SetNEDVelFpsIC(eU, vn); }

  /** Sets the initial local axis east velocity.
      @param ve Initial east velocity in feet/second */
  void SetVEastFpsIC(Real ve) { SetNEDVelFpsIC(eV, ve); }

  /** Sets the initial local axis down velocity.
      @param vd Initial down velocity in feet/second */
  void SetVDownFpsIC(Real vd) { SetNEDVelFpsIC(eW, vd); }

  /** Sets the initial body axis roll rate.
      @param P Initial roll rate in radians/second */
  void SetPRadpsIC(Real P)  { vPQR_body(eP) = P; }

  /** Sets the initial body axis pitch rate.
      @param Q Initial pitch rate in radians/second */
  void SetQRadpsIC(Real Q) { vPQR_body(eQ) = Q; }

  /** Sets the initial body axis yaw rate.
      @param R initial yaw rate in radians/second */
  void SetRRadpsIC(Real R) { vPQR_body(eR) = R; }

  /** Sets the initial wind velocity.
      @param wN Initial wind velocity in local north direction, feet/second
      @param wE Initial wind velocity in local east direction, feet/second
      @param wD Initial wind velocity in local down direction, feet/second   */
  void SetWindNEDFpsIC(Real wN, Real wE, Real wD);

  /** Sets the initial total wind speed.
      @param mag Initial wind velocity magnitude in knots */
  void SetWindMagKtsIC(Real mag);

  /** Sets the initial wind direction.
      @param dir Initial direction wind is coming from in degrees */
  void SetWindDirDegIC(Real dir);

  /** Sets the initial headwind velocity.
      @param head Initial headwind speed in knots */
  void SetHeadWindKtsIC(Real head);

  /** Sets the initial crosswind speed.
      @param cross Initial crosswind speed, positive from left to right */
  void SetCrossWindKtsIC(Real cross);

  /** Sets the initial wind downward speed.
      @param wD Initial downward wind speed in knots*/
  void SetWindDownKtsIC(Real wD);

  /** Sets the initial climb rate.
      @param roc Initial Rate of climb in feet/second */
  void SetClimbRateFpsIC(Real roc);

  /** Gets the initial ground velocity.
      @return Initial ground velocity in feet/second */
  Real GetVgroundFpsIC(void) const  { return vUVW_NED.Magnitude(eU, eV); }

  /** Gets the initial true velocity.
      @return Initial true velocity in feet/second */
  Real GetVtrueFpsIC(void) const { return vt; }

  /** Gets the initial body axis X wind velocity.
      @return Initial body axis X wind velocity in feet/second */
  Real GetWindUFpsIC(void) const { return GetBodyWindFpsIC(eU); }

  /** Gets the initial body axis Y wind velocity.
      @return Initial body axis Y wind velocity in feet/second */
  Real GetWindVFpsIC(void) const { return GetBodyWindFpsIC(eV); }

  /** Gets the initial body axis Z wind velocity.
      @return Initial body axis Z wind velocity in feet/second */
  Real GetWindWFpsIC(void) const { return GetBodyWindFpsIC(eW); }

  /** Gets the initial wind velocity in the NED local frame
      @return Initial wind velocity in NED frame in feet/second */
  const FGColumnVector3 GetWindNEDFpsIC(void) const {
    const FGMatrix33& Tb2l = orientation.GetTInv();
    FGColumnVector3 _vt_NED = Tb2l * Tw2b * FGColumnVector3(vt, 0., 0.);
    return _vt_NED - vUVW_NED;
  }

  /** Gets the initial wind velocity in local frame.
      @return Initial wind velocity toward north in feet/second */
  Real GetWindNFpsIC(void) const { return GetNEDWindFpsIC(eX); }

  /** Gets the initial wind velocity in local frame.
      @return Initial wind velocity eastwards in feet/second */
  Real GetWindEFpsIC(void) const { return GetNEDWindFpsIC(eY); }

  /** Gets the initial wind velocity in local frame.
      @return Initial wind velocity downwards in feet/second */
  Real GetWindDFpsIC(void) const { return GetNEDWindFpsIC(eZ); }

  /** Gets the initial total wind velocity in feet/sec.
      @return Initial wind velocity in feet/second */
  Real GetWindFpsIC(void)  const;

  /** Gets the initial wind direction.
      @return Initial wind direction in feet/second */
  Real GetWindDirDegIC(void) const;

  /** Gets the initial climb rate.
      @return Initial rate of climb in feet/second */
  Real GetClimbRateFpsIC(void) const
  {
    const FGMatrix33& Tb2l = orientation.GetTInv();
    FGColumnVector3 _vt_NED = Tb2l * Tw2b * FGColumnVector3(vt, 0., 0.);
    return -_vt_NED(eW);
  }

  /** Gets the initial body velocity
      @return Initial body velocity in feet/second. */
  const FGColumnVector3 GetUVWFpsIC(void) const {
    const FGMatrix33& Tl2b = orientation.GetT();
    return Tl2b * vUVW_NED;
  }

  /** Gets the initial body axis X velocity.
      @return Initial body axis X velocity in feet/second. */
  Real GetUBodyFpsIC(void) const { return GetBodyVelFpsIC(eU); }

  /** Gets the initial body axis Y velocity.
      @return Initial body axis Y velocity in feet/second. */
  Real GetVBodyFpsIC(void) const { return GetBodyVelFpsIC(eV); }

  /** Gets the initial body axis Z velocity.
      @return Initial body axis Z velocity in feet/second. */
  Real GetWBodyFpsIC(void) const { return GetBodyVelFpsIC(eW); }

  /** Gets the initial local frame X (North) velocity.
      @return Initial local frame X (North) axis velocity in feet/second. */
  Real GetVNorthFpsIC(void) const { return vUVW_NED(eU); }

  /** Gets the initial local frame Y (East) velocity.
      @return Initial local frame Y (East) axis velocity in feet/second. */
  Real GetVEastFpsIC(void) const { return vUVW_NED(eV); }

  /** Gets the initial local frame Z (Down) velocity.
      @return Initial local frame Z (Down) axis velocity in feet/second. */
  Real GetVDownFpsIC(void) const { return vUVW_NED(eW); }

  /** Gets the initial body rotation rate
      @return Initial body rotation rate in radians/second */
  const FGColumnVector3 GetPQRRadpsIC(void) const { return vPQR_body; }

  /** Gets the initial body axis roll rate.
      @return Initial body axis roll rate in radians/second */
  Real GetPRadpsIC() const { return vPQR_body(eP); }

  /** Gets the initial body axis pitch rate.
      @return Initial body axis pitch rate in radians/second */
  Real GetQRadpsIC() const { return vPQR_body(eQ); }

  /** Gets the initial body axis yaw rate.
      @return Initial body axis yaw rate in radians/second */
  Real GetRRadpsIC() const { return vPQR_body(eR); }

  /** Sets the initial flight path angle.
      @param gamma Initial flight path angle in radians */
  void SetFlightPathAngleRadIC(Real gamma)
  { SetClimbRateFpsIC(vt*sin(gamma)); }

  /** Sets the initial angle of attack.
      @param alpha Initial angle of attack in radians */
  void SetAlphaRadIC(Real alpha);

  /** Sets the initial sideslip angle.
      @param beta Initial angle of sideslip in radians. */
  void SetBetaRadIC(Real beta);

  /** Sets the initial roll angle.
      @param phi Initial roll angle in radians */
  void SetPhiRadIC(Real phi) { SetEulerAngleRadIC(ePhi, phi); }

  /** Sets the initial pitch angle.
      @param theta Initial pitch angle in radians */
  void SetThetaRadIC(Real theta) { SetEulerAngleRadIC(eTht, theta); }

  /** Sets the initial heading angle.
      @param psi Initial heading angle in radians */
  void SetPsiRadIC(Real psi) { SetEulerAngleRadIC(ePsi, psi); }

  /** Sets the initial latitude.
      @param lat Initial latitude in radians */
  void SetLatitudeRadIC(Real lat);

  /** Sets the initial geodetic latitude.
      This method modifies the geodetic altitude in order to keep the altitude
      above sea level unchanged.
      @param glat Initial geodetic latitude in radians */
  void SetGeodLatitudeRadIC(Real glat);

  /** Sets the initial longitude.
      @param lon Initial longitude in radians */
  void SetLongitudeRadIC(Real lon);

  /** Sets the target normal load factor.
      @param nlf Normal load factor*/
  void SetTargetNlfIC(Real nlf) { targetNlfIC=nlf; }

  /** Gets the initial flight path angle.
      If total velocity is zero, this function returns zero.
      @return Initial flight path angle in radians */
  Real GetFlightPathAngleRadIC(void) const
  { return (vt == 0.0)?Real(0.0):asin(GetClimbRateFpsIC() / vt); }

  /** Gets the initial angle of attack.
      @return Initial alpha in radians */
  Real GetAlphaRadIC(void) const { return alpha; }

  /** Gets the initial angle of sideslip.
      @return Initial sideslip angle in radians */
  Real GetBetaRadIC(void) const { return beta; }

  /** Gets the initial position
      @return Initial location */
  const FGLocation& GetPosition(void) const { return position; }

  /** Gets the initial latitude.
      @return Initial latitude in radians */
  Real GetLatitudeRadIC(void) const { return position.GetLatitude(); }

  /** Gets the initial geodetic latitude.
      @return Initial geodetic latitude in radians */
  Real GetGeodLatitudeRadIC(void) const
  { return position.GetGeodLatitudeRad(); }

  /** Gets the initial longitude.
      @return Initial longitude in radians */
  Real GetLongitudeRadIC(void) const { return position.GetLongitude(); }

  /** Gets the initial orientation
      @return Initial orientation */
  const FGQuaternion& GetOrientation(void) const { return orientation; }

  /** Gets the initial roll angle.
      @return Initial roll angle in radians */
  Real GetPhiRadIC(void) const { return orientation.GetEuler(ePhi); }

  /** Gets the initial pitch angle.
      @return Initial pitch angle in radians */
  Real GetThetaRadIC(void) const { return orientation.GetEuler(eTht); }

  /** Gets the initial heading angle.
      @return Initial heading angle in radians */
  Real GetPsiRadIC(void) const   { return orientation.GetEuler(ePsi); }

  /** Gets the initial speedset.
      @return Initial speedset */
  speedset GetSpeedSet(void) const { return lastSpeedSet; }

  /** Gets the target normal load factor set from IC.
      @return target normal load factor set from IC*/
  Real GetTargetNlfIC(void) const { return targetNlfIC; }

  /** Loads the initial conditions.
      @param rstname The name of an initial conditions file
      @param useStoredPath true if the stored path to the IC file should be used
      @return true if successful */
  bool Load(const SGPath& rstname, bool useStoredPath = true );

  /** Is an engine running ?
      @param index of the engine to be checked
      @return true if the engine is running. */
  bool IsEngineRunning(unsigned int n) const { return (enginesRunning & (1 << n)) != 0; }
  
  /** Does initialization file call for trim ?
      @return Trim type, if any requested (version 1). */
  int TrimRequested(void) const { return trimRequested; }

  /** Initialize the initial conditions to default values */
  void InitializeIC(void);

  void bind(FGPropertyManager* pm);

private:
  FGColumnVector3 vUVW_NED;
  FGColumnVector3 vPQR_body;
  FGLocation position;
  FGQuaternion orientation;
  Real vt;

  Real targetNlfIC;

  FGMatrix33 Tw2b, Tb2w;
  Real alpha, beta;
  Real epa;

  speedset lastSpeedSet;
  altitudeset lastAltitudeSet;
  latitudeset lastLatitudeSet;
  unsigned int enginesRunning;
  int trimRequested;

  FGFDMExec *fdmex;
  std::shared_ptr<FGAtmosphere> Atmosphere;
  std::shared_ptr<FGAircraft> Aircraft;

  bool Load_v1(Element* document);
  bool Load_v2(Element* document);

  void SetEulerAngleRadIC(int idx, Real angle);
  void SetBodyVelFpsIC(int idx, Real vel);
  void SetNEDVelFpsIC(int idx, Real vel);
  Real GetBodyWindFpsIC(int idx) const;
  Real GetNEDWindFpsIC(int idx) const;
  Real GetBodyVelFpsIC(int idx) const;
  void calcAeroAngles(const FGColumnVector3& _vt_BODY);
  void calcThetaBeta(Real alfa, const FGColumnVector3& _vt_NED);
  Real ComputeGeodAltitude(Real geodLatitude);
  bool LoadLatitude(Element* position_el);
  void SetTrimRequest(std::string trim);
  void Debug(int from);
};
}
#endif
