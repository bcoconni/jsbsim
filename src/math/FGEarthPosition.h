/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Header:       FGEarthPosition.h
 Author:       Bertrand Coconnier
 Date started: 07/29/15

 ------------- Copyright (C) 2015  Bertrand Coconnier -------------

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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef FGEARTHPOSITION_H
#define FGEARTHPOSITION_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <cmath>
#include "FGMatrix33.h"
#include "FGJSBBase.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#define ID_EARTHPOSITION "$Id$"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace JSBSim {

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class FGEarthPosition
{
public:
  explicit FGEarthPosition(double rotation_rate)
    : RotationAxis(0.0, 0.0, rotation_rate), epa(0.0) {}

  /** Return the Earth Position Angle in radians.
      This is the relative orientation of the ECEF frame with respect to the
      Inertial frame.
      @return the Earth fixed frame (ECEF) rotation offset about the axis with
              respect to the Inertial (ECI) frame in radians. */
  double GetAngle(void) const { return epa; }
  double GetAngleDeg(void) const { return epa * FGJSBBase::radtodeg; }

  /** Sets the Earth position angle.
      This is the relative orientation of the ECEF frame with respect to the
      Inertial frame.
      @param angle Earth fixed frame (ECEF) rotation offset about the axis with
                   respect to the Inertial (ECI) frame in radians. */
  void SetAngle(double angle) { epa = angle; UpdateTransformMatrices(); }

  /** Increments the Earth position angle.
      This is the relative orientation of the ECEF frame with respect to the
      Inertial frame. The Earth rotation rate is constant and the angle is
      incremented by the time increment times the rotation rate.
      @param dt time increment in seconds. */
  void IncrementAngle(double dt) { epa += dt*RotationAxis(3); UpdateTransformMatrices(); }

  /** Retrieves the transform matrix from the inertial to the earth centered
      frame.
      @return a const reference to the rotation matrix of the transform from
      the inertial frame to the earth centered frame (ECI to ECEF).
      @see GetAngle
      @see SetAngle */
  const FGMatrix33& GetTi2ec(void) const { return Ti2ec; }

  /** Retrieves the transform matrix from the earth centered to the inertial
      frame.
      @return a const reference to the rotation matrix of the transform from
      the earth centered frame to the inertial frame (ECEF to ECI).
      @see GetAngle
      @see SetAngle */
  const FGMatrix33& GetTec2i(void) const { return Tec2i; }
  const FGColumnVector3& GetRotationAxis(void) const { return RotationAxis; }

private:
  void UpdateTransformMatrices(void)
  {
    double cos_epa = cos(epa);
    double sin_epa = sin(epa);
    Ti2ec = FGMatrix33( cos_epa,  sin_epa, 0.0,
                        -sin_epa, cos_epa, 0.0,
                        0.0,      0.0,     1.0 ); // ECI to ECEF transform
    Tec2i = Ti2ec.Transposed();                   // ECEF to ECI frame transform
  }

  FGColumnVector3 RotationAxis;
  FGMatrix33 Ti2ec;
  FGMatrix33 Tec2i;
  double epa; // Earth Position Angle
};
} // namespace

#endif
