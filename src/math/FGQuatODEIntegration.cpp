/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Module: FGQuatODEIntegration.cpp
Author: Bertrand Coconnier
Date started: 08/23/2015
Purpose: Integrates quaternion differential equations

 ------------- Copyright (C) 2015 Bertrand Coconnier  -------------

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
COMMENTS, REFERENCES,  and NOTES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[1] S. Buss, "Accurate and Efficient Simulation of Rigid Body Rotations",
    Technical Report, Department of Mathematics, University of California,
    San Diego, 1999
[2] Barker L.E., Bowles R.L. and Williams L.H., "Development and Application of
    a Local Linearization Algorithm for the Integration of Quaternion Rate
    Equations in Real-Time Flight Simulation Problems", NASA TN D-7347,
    December 1973
[3] Phillips W.F, Hailey C.E and Gebert G.A, "Review of Attitude Representations
    Used for Aircraft Kinematics", Journal Of Aircraft Vol. 38, No. 4,
    July-August 2001

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "FGMatrix33.h"
#include "FGQuatODEIntegration.h"

using namespace std;

namespace JSBSim {

IDENT(IdSrc,"$Id$");
IDENT(IdHdr,ID_QUATODEINTEGRATION);

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void FGQuatODEIntegration::Propagate(void)
{
  switch(method) {
  case eBuss1:
  case eBuss2:
  case eLocalLinearization:
    v0 *= dq;
    return;
  default:
    FGMultiStepMethod<FGQuaternion>::Propagate();
    v0.Normalize();
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGQuaternion FGQuatODEIntegration::integrate(const FGColumnVector3& omegaECI,
                                             const FGColumnVector3& omegadotECI)
{
  switch(method) {
  case eBuss1:
    {
      // This is the first order method as described in Samuel R. Buss paper[1].
      // The formula from Buss' paper is transposed below to quaternions and is
      // actually the exact solution of the quaternion differential equation
      // qdot = 1/2*w*q when w is constant.
      dq = QExp(0.5 * dt * omegaECI);
      return v0 * dq;
    }
  case eBuss2:
    {
      // This is the 'augmented second-order method' from S.R. Buss paper [1].
      // Unlike Runge-Kutta or Adams-Bashforth, it is a one-pass second-order
      // method (see reference [1]).
      FGColumnVector3 wdoti = (0.5 * dt) * omegadotECI;
      FGColumnVector3 omega = omegaECI + wdoti + (dt/6.0)*wdoti*omegaECI;
      dq = QExp(0.5 * dt * omega);
      return v0 * dq;
    }
  case eLocalLinearization:
    {
      // This is the local linearization algorithm of Barker et al. (see
      // ref. [2]) It is also a one-pass second-order method. The code below is
      // based on the more compact formulation issued from equation (107) of
      // ref. [3]. The constants C1, C2, C3 and C4 have the same value than
      // those in ref. [2] pp. 11
      FGColumnVector3 wi = 0.5 * omegaECI;
      FGColumnVector3 wdoti = 0.5 * omegadotECI;
      double omegak2 = DotProduct(omegaECI, omegaECI);
      double omegak = omegak2 > 1E-6 ? sqrt(omegak2) : 1E-6;
      double rhok = 0.5 * dt * omegak;
      double C1 = cos(rhok);
      double C2 = 2.0 * sin(rhok) / omegak;
      double C3 = 4.0 * (1.0 - C1) / (omegak*omegak);
      double C4 = 4.0 * (dt - C2) / (omegak*omegak);
      FGColumnVector3 Omega = C2*wi + C3*wdoti + C4*wi*wdoti;
      dq = FGQuaternion(C1 - C4*DotProduct(wi, wdoti), Omega(1), Omega(2),
                        Omega(3));

      /* Cross check with ref. [7] pp.11-12 formulas and code pp. 20
         double pk = VState.vPQRi(eP);
         double qk = VState.vPQRi(eQ);
         double rk = VState.vPQRi(eR);
         double pdotk = in.vPQRidot(eP);
         double qdotk = in.vPQRidot(eQ);
         double rdotk = in.vPQRidot(eR);
         double Ap = -0.25 * (pk*pdotk + qk*qdotk + rk*rdotk);
         double Bp = 0.25 * (pk*qdotk - qk*pdotk);
         double Cp = 0.25 * (pdotk*rk - pk*rdotk);
         double Dp = 0.25 * (qk*rdotk - qdotk*rk);
         double C2p = sin(rhok) / omegak;
         double C3p = 2.0 * (1.0 - cos(rhok)) / (omegak*omegak);
         double H = C1 + C4 * Ap;
         double G = -C2p*rk - C3p*rdotk + C4*Bp;
         double J = C2p*qk + C3p*qdotk - C4*Cp;
         double K = C2p*pk + C3p*pdotk - C4*Dp;

         cout << "q:       " << q << endl;

         // Warning! In the paper of Barker et al. the quaternion components are
         // not ordered the same way as in JSBSim (see equations (2) and (3) of
         // ref. [7] as well as the comment just below equation (3)) cout <<
         // "FORTRAN: " << H << " , " << K << " , " << J << " , " << -G <<
         // endl;*/
      return v0 * dq;
    }
  default:
    {
      // vQtrndot is the quaternion derivative on current body rates.
      // Reference: See Stevens and Lewis, "Aircraft Control and Simulation",
      //            Second edition (2004), eqn 1.5-16b (page 50)
      FGQuaternion vQtrndot = v0.GetQDot(omegaECI);
      FGQuaternion q = FGMultiStepMethod<FGQuaternion>::integrate(vQtrndot);
      q.Normalize();
      return q;
    }
  }
}
}
