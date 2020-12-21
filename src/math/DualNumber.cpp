/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Header: DualNumber.cpp
Author: Bertrand Coconnier
Date started: November 29 2020

 ------------- Copyright (C) 2020  Bertrand Coconnier -------------

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
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "DualNumber.h"

namespace JSBSim {

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGDualNumber asin(FGDualNumber x) {
  double v = std::asin(x.value);
  if (fabs(x.value) != 1.0)
    return FGDualNumber(v, x.gradient / std::sqrt(1 - x.value * x.value));
  if (x.gradient == 0.0)
    return FGDualNumber(v, 0.0); // The gradient has no significance avoid raising an FPE.
  return FGDualNumber(v, std::numeric_limits<double>::infinity());
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGDualNumber acos(FGDualNumber x) {
  double v = std::acos(x.value);
  if (fabs(x.value) != 1.0)
    return FGDualNumber(v, -x.gradient / std::sqrt(1 - x.value * x.value));
  if (x.gradient == 0.0)
    return FGDualNumber(v, 0.0); // The gradient has no significance avoid raising an FPE.
  return FGDualNumber(v, -std::numeric_limits<double>::infinity());
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGDualNumber pow(const FGDualNumber& x, double y) {
  if (x.value != 0.0)
    return FGDualNumber(std::pow(x.value, y), y * x.gradient * std::pow(x.value, y-1.0));
  else {
    if (y >= 1.0)
      return y > 1.0 ? FGDualNumber(0.0, 0.0) : FGDualNumber(0.0, x.gradient);
    if (y < 0.0)
      return FGDualNumber(std::numeric_limits<double>::infinity(),
                          std::numeric_limits<double>::signaling_NaN());
     if (x.gradient == 0.0)
       return y == 0.0 ? FGDualNumber(1.0, 0.0) : FGDualNumber(0.0, 0.0);
     return y == 0.0 ? FGDualNumber(1.0, std::numeric_limits<double>::infinity())
                     : FGDualNumber(0.0, std::numeric_limits<double>::signaling_NaN());
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGDualNumber sqrt(const FGDualNumber& x) {
  if (x.value != 0.0) {
    double sqr = std::sqrt(x.value);
    return FGDualNumber(sqr, 0.5*x.gradient/sqr);
  }
  if (x.gradient == 0.0)
    return FGDualNumber(0.0, 0.0);
  return FGDualNumber(0.0, std::numeric_limits<double>::infinity());
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGDualNumber atan2(const FGDualNumber& y, const FGDualNumber& x) {
  double v = std::atan2(y.value, x.value);
  if (x.value != 0.0 && y.value != 0.0)
    return FGDualNumber(v, (y.gradient * x.value - y.value * x.gradient) / (x.value * x.value + y.value * y.value));
  if (x.gradient == 0.0 && y.gradient == 0.0)
    return FGDualNumber(v, 0.0);
  return FGDualNumber(v, std::numeric_limits<double>::infinity());
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGDualNumber fabs(FGDualNumber x) {
  if (x.value >= 0.0)
    return FGDualNumber(x.value, x.gradient);
  else
    return FGDualNumber(-x.value, -x.gradient);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGDualNumber modf(FGDualNumber x, FGDualNumber* y)
{
  double fraction, integer;
  fraction = std::modf(x.value, &integer);
  *y = {integer, 0.};
  return FGDualNumber(fraction, 1.);
}
} // namespace JSBSim
