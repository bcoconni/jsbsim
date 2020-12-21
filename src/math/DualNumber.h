/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Header: DualNumber.h
Author: Bertrand Coconnier
Date started: November 10 2012

 ------------- Copyright (C) 2012-2020  Bertrand Coconnier -------------

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

#ifndef FGDUALNUMBER_H
#define FGDUALNUMBER_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <limits>
#include <cmath>
#include <iostream>
#include "simgear/props/props.hxx"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace JSBSim {

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

  /** Represents a dual number that will be used in automatic differentiation
      @author Bertrand Coconnier
  */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DECLARATION: FGDualNumber
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class FGDualNumber
{
public:

  constexpr FGDualNumber(void) : value(0.0), gradient(0.0) {}
  constexpr FGDualNumber(double a) : value(a), gradient(0.0) {}
  constexpr FGDualNumber(double a, double b) : value(a), gradient(b) {}

  constexpr operator double () const { return value; }
  constexpr double getValue(void) const { return value; }
  constexpr double getGradient(void) const { return gradient; }

  constexpr FGDualNumber operator-() const {
    return FGDualNumber(-value, -gradient);
  }
  constexpr FGDualNumber operator+(const FGDualNumber& x) const {
    return FGDualNumber(value + x.value, gradient + x.gradient);
  }
  FGDualNumber& operator+=(const FGDualNumber& x) {
    value += x.value;
    gradient += x.gradient;
    return *this;
  }
  constexpr FGDualNumber operator-(const FGDualNumber& x) const {
    return FGDualNumber(value - x.value, gradient - x.gradient);
  }
  FGDualNumber& operator-=(const FGDualNumber& x) {
    value -= x.value;
    gradient -= x.gradient;
    return *this;
  }
  constexpr FGDualNumber operator*(const FGDualNumber& x) const {
    return FGDualNumber(value * x.value, value * x.gradient + gradient * x.value);
  }
  constexpr FGDualNumber operator/(const FGDualNumber& x) const {
    double x2 = x.value * x.value;
    return FGDualNumber(value / x.value, (gradient * x.value - value * x.gradient) / x2);
  }
  FGDualNumber& operator/=(const FGDualNumber& x) {
    double x2 = x.value * x.value;
    gradient = (gradient * x.value - value * x.gradient) / x2;
    value /= x.value;
    return *this;
  }
  constexpr bool operator==(const FGDualNumber& x) const {
    return value == x.value;
  }
  constexpr bool operator!=(const FGDualNumber& x) const {
    return value != x.value;
  }
  constexpr bool operator<=(const FGDualNumber& x) const {
    return value <= x.value;
  }
  constexpr bool operator>=(const FGDualNumber& x) const {
    return value >= x.value;
  }
  constexpr bool operator>(const FGDualNumber& x) const {
    return value > x.value;
  }
  constexpr bool operator<(const FGDualNumber& x) const {
    return value < x.value;
  }
  FGDualNumber& operator*=(const FGDualNumber& x) {
    gradient = value * x.gradient + gradient * x.value;
    value *= x.value;
    return *this;
  }

  /* operators with numeric types */
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, FGDualNumber>
  operator+(T a) const {
    return FGDualNumber(value + a, gradient);
  }
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, FGDualNumber>
  operator-(T a) const {
    return FGDualNumber(value - a, gradient);
  }
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, FGDualNumber>
  operator*(T a) const {
    return FGDualNumber(value * a, gradient * a);
  }
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, FGDualNumber>&
  operator*=(T a) {
    value *= a;
    gradient *= a;
    return *this;
  }
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, FGDualNumber>
  operator/(T a) const {
    return FGDualNumber(value / a, gradient / a);
  }
  FGDualNumber operator++(int) {
    FGDualNumber current = *this;
    value++;
    return current;
  }
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, bool>
  operator==(T a) const {
    return value == a;
  }
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, bool>
  operator!=(T a) const {
    return value != a;
  }
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, bool>
  operator<(T a) const {
    return value < a;
  }
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, bool>
  operator<=(T a) const {
    return value <= a;
  }
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, bool>
  operator>(T a) const {
    return value > a;
  }
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, bool>
  operator>=(T a) const {
    return value >= a;
  }
  /* friend functions */
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, FGDualNumber>
  friend operator/(T a, const FGDualNumber& x) {
    double x2 = x.value * x.value;
    return FGDualNumber(a / x.value, -a * x.gradient / x2);
  }
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, FGDualNumber>
  friend operator-(T a, const FGDualNumber& x) {
    return FGDualNumber(a - x.value, -x.gradient);
  }
  template <typename T> constexpr
  std::enable_if_t<std::is_arithmetic<T>::value, FGDualNumber>
  friend operator+(T a, const FGDualNumber& x) {
    return FGDualNumber(a + x.value, x.gradient);
  }
  friend FGDualNumber sqrt(const FGDualNumber& x);
  friend FGDualNumber asin(FGDualNumber x);
  friend FGDualNumber acos(FGDualNumber x);
  friend FGDualNumber atan2(const FGDualNumber& y, const FGDualNumber& x);
  friend FGDualNumber cos(FGDualNumber x);
  friend FGDualNumber sin(FGDualNumber x);
  friend FGDualNumber tan(FGDualNumber x);
  friend FGDualNumber fabs(FGDualNumber x);
  friend FGDualNumber pow(const FGDualNumber& x, double y);
  friend FGDualNumber atan(FGDualNumber x);
  friend FGDualNumber exp(FGDualNumber x);
  friend FGDualNumber log(const FGDualNumber& x) {
    return FGDualNumber(std::log(x.value), x.gradient/x.value);
  }
  friend FGDualNumber log10(const FGDualNumber& x) {
    return FGDualNumber(std::log10(x.value), x.gradient/(x.value*std::log(10.)));
  }
  friend FGDualNumber floor(FGDualNumber x);
  friend FGDualNumber ceil(FGDualNumber x);
  friend FGDualNumber modf(FGDualNumber x, FGDualNumber* y);
  friend std::ostream& operator<<(std::ostream& os, const FGDualNumber& x) {
    os << x.value;
    return os;
  }
  friend std::istream& operator>>(std::istream& is, FGDualNumber& x) {
    is >> x.value;
    return is;
  }

private:
  double value;
  double gradient;
};

template <typename T> inline
std::enable_if_t<std::is_arithmetic<T>::value, FGDualNumber>
max(const FGDualNumber& x, T a) {
  return x >= a ? x : FGDualNumber(a);
}
template <typename T> inline
std::enable_if_t<std::is_arithmetic<T>::value, FGDualNumber>
max(T a, const FGDualNumber& x) {
  return x >= a ? x : FGDualNumber(a);
}
template <typename T> inline
std::enable_if_t<std::is_arithmetic<T>::value, FGDualNumber>
min(const FGDualNumber& x, T a) {
  return x <= a ? x : FGDualNumber(a);
}
template <typename T> inline
std::enable_if_t<std::is_arithmetic<T>::value, FGDualNumber>
min(T a, const FGDualNumber& x) {
  return x <= a ? x : FGDualNumber(a);
}

inline FGDualNumber exp(FGDualNumber x) {
    double value = std::exp(x.value);
    return FGDualNumber(value, x.gradient * value);
}

FGDualNumber fabs(FGDualNumber x);
FGDualNumber asin(FGDualNumber x);
FGDualNumber acos(FGDualNumber x);
FGDualNumber pow(const FGDualNumber& x, double y);
FGDualNumber sqrt(const FGDualNumber& x);
FGDualNumber atan2(const FGDualNumber& x, const FGDualNumber& y);
FGDualNumber modf(FGDualNumber x, FGDualNumber* y);

inline FGDualNumber cos(FGDualNumber x) {
    return FGDualNumber(std::cos(x.value), -x.gradient * std::sin(x.value));
}

inline FGDualNumber sin(FGDualNumber x) {
    return FGDualNumber(std::sin(x.value), x.gradient * std::cos(x.value));
}

inline FGDualNumber tan(FGDualNumber x) {
    double value = std::tan(x.value);
    return FGDualNumber(value, x.gradient * (1.0 + value * value));
}

inline FGDualNumber atan(FGDualNumber x) {
    return FGDualNumber(std::atan(x.value), x.gradient / (1.0 + x.value*x.value));
}

inline FGDualNumber floor(FGDualNumber x) {
    // The function floor is constant everywhere but at integer values where
    // its derivative is infinite ?
    // Here, we are assuming that its derivative is zero everywhere.
    return FGDualNumber(std::floor(x.value), 0.);
}
inline FGDualNumber ceil(FGDualNumber x) {
    // The function floor is constant everywhere but at integer values where
    // its derivative is infinite ?
    // Here, we are assuming that its derivative is zero everywhere.
    return FGDualNumber(std::ceil(x.value), 0.);
}
} // namespace JSBSim

namespace simgear {
  namespace props {
    template<> struct PropertyTraits<JSBSim::FGDualNumber>
    {
      static const Type type_tag = EXTENDED;
      enum { Internal = 0 };
    };
  }
}

#endif
