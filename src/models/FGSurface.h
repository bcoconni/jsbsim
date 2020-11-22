/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Header:       FGSurface.h
 Author:       Erik Hofman
 Date started: 01/15/14

 ------------- Copyright (C) 2014  Jon S. Berndt (jon@jsbsim.org) -------------

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
01/15/14   EMH   Created

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef FGSURFACE_H
#define FGSURFACE_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace JSBSim {

class FGFDMExec;
class FGPropertyManager;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** Base class for all surface properties
    @author Erik M. Hofman
  */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class FGSurface
{
public:

  enum ContactType {ctBOGEY, ctSTRUCTURE, ctGROUND};

  /// Constructor
  FGSurface(FGFDMExec* fdmex, int number = -1);

  /// Destructor
  ~FGSurface();

  void bind(FGPropertyManager* pm);

  /// Reset all surface values to a default
  void resetValues(void);

  /// Sets the static friction factor of the surface area
  void SetStaticFFactor(Real friction) { staticFFactor = friction; }

  /// Sets the rolling friction factor of the surface area
  void SetRollingFFactor(Real friction) { rollingFFactor = friction; }

  /// Sets the maximum force for the surface area
  void SetMaximumForce(Real force) { maximumForce = force; }

  /// Sets the normalized bumpiness factor associated with the surface
  void SetBumpiness(Real bump) { bumpiness = bump; }

  /// Sets the surface is a solid flag value
  void SetSolid(bool solid) { isSolid = solid; }

  /// Set the currect position for bumpiness calulcation
  void SetPosition(const Real pt[3]) {
      pos[0] = pt[0]; pos[1] = pt[1]; pos[2] = pt[2];
  }


  /// Gets the static friction factor of the surface area
  Real GetStaticFFactor(void) { return staticFFactor; }

  /// Gets the rolling friction factor of the surface area
  Real GetRollingFFactor(void) { return rollingFFactor; }

  /// Gets the maximum force of the surface area
  Real GetMaximumForce(void) { return maximumForce; }

  /// Gets the normalized bumpiness factor associated with the surface
  Real GetBumpiness(void) { return bumpiness; }

  /// Gets the surface is a solid flag value
  bool GetSolid(void) { return isSolid; }

  /// Returns the height of the bump at the provided offset
  Real  GetBumpHeight();

  std::string GetSurfaceStrings(std::string delimeter) const;
  std::string GetSurfaceValues(std::string delimeter) const;

protected:
  ContactType eSurfaceType;
  Real staticFFactor, rollingFFactor;
  Real maximumForce;
  Real bumpiness;
  bool isSolid;

  Real staticFCoeff, dynamicFCoeff;

private:
  int contactNumber;
  Real pos[3];

  static std::string _CreateIndexedPropertyName(const std::string& Property, int index);
};
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#endif
