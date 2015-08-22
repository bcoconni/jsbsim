/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Header:       FGMultiStepMethod.h
 Author:       Bertrand Coconnier
 Date started: 08/15/15

 ------------- Copyright (C) 2015  Bertrand Coconnier -------------

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; either version 2 of the License, or (at your option) any
 later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along
 with this program; if not, write to the Free Software Foundation, Inc., 59
 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

 Further information about the GNU Lesser General Public License can also be
 found on the world wide web at http://www.gnu.org.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef FGMULTISTEPMETHOD_H
#define FGMULTISTEPMETHOD_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <deque>
#include "FGTimeMarching.h"
#include "FGQuaternion.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#define ID_MULTISTEPMETHOD "$Id$"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace JSBSim
{

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/// These define the indices use to select the various integrators.
enum eIntegrateType {eNone = 0, eRectEuler, eTrapezoidal, eAdamsBashforth2,
                     eAdamsBashforth3, eAdamsBashforth4, eBuss1, eBuss2,
                     eLocalLinearization, eAdamsBashforth5};

template<class T> class FGMultiStepMethod : public FGTimeMarching
{
public:
  FGMultiStepMethod(FGPropagate* pg)
    : FGTimeMarching(pg), step(0), method(eRectEuler) {}

  void Update(void) { v0 += dv; }
  void setMethod(int t) { method = (eIntegrateType)t; }

  int getMethod(void) const { return (int)method; }
  void setInitialCondition(const T& v) { v0 = v; }
  void setInitialDerivative(const T& ICdot) {
    valDot.assign(5, ICdot);
    step = 0;
  }

  virtual T integrate(const T& dot) {
    if (dt > 0.) {
      valDot.push_front(dot);
      valDot.pop_back();

      switch(method) {
      case eRectEuler:
        dv = dt * valDot[0];
        break;
      case eAdamsBashforth2:
        if (step == 0) {
          ++step;
          dv = dt * valDot[0];
          Notify();
          break;
        }
        else if (step == 1) {
          ++step;
          valDot.pop_front();
          valDot.push_back(valDot.back());
          dv = 0.5 * dt * (dot + valDot[0]);
          break;
        }
        dv = dt * (1.5 * valDot[0] - 0.5 * valDot[1]);
        break;
      case eAdamsBashforth3:
        dv = (dt / 12.0) * (23.0 * valDot[0] - 16.0 * valDot[1] + 5.0 * valDot[2]);
        break;
      case eAdamsBashforth4:
        dv = (dt / 24.0) * (55.0 * valDot[0] - 59.0 * valDot[1] + 37.0 * valDot[2] - 9.0 * valDot[3]);
        break;
      case eAdamsBashforth5:
        dv = (dt / 720.) * (1901.0 * valDot[0] - 2774.0 * valDot[1] + 2616.0 * valDot[2] - 1274.0 * valDot[3] + 251.0 * valDot[4]);
        break;
      default:
        break;
      }
      return v0 + dv;
    }

    dv = dt * dv;
    return v0;
  }

protected:
  unsigned int step;
  T v0, dv;
  eIntegrateType method;
  std::deque<T> valDot;
};

class FGMultiStepMethodQ : public FGMultiStepMethod<FGQuaternion>
{
public:
  FGMultiStepMethodQ(FGPropagate* pg) : FGMultiStepMethod<FGQuaternion>(pg) {}
  void Update(void) {
    FGMultiStepMethod<FGQuaternion>::Update();
    v0.Normalize();
  }
  FGQuaternion integrate(const FGQuaternion& dot) {
    FGQuaternion q = FGMultiStepMethod<FGQuaternion>::integrate(dot);
    q.Normalize();
    return q;
  }
};
} // namespace

#endif
