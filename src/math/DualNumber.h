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

#define ID_DUALNUMBER "$Id$"

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

  constexpr FGDualNumber(void) : real(0.0), diff(0.0) {}
  constexpr FGDualNumber(double a) : real(a), diff(0.0) {}
  constexpr FGDualNumber(double a, double b) : real(a), diff(b) {}

  constexpr operator double () const { return real; }

  constexpr FGDualNumber operator-() const {
    return FGDualNumber(-real, -diff);
  }
  constexpr FGDualNumber operator+(const FGDualNumber& x) const {
    return FGDualNumber(real + x.real, diff + x.diff);
  }
  FGDualNumber& operator+=(const FGDualNumber& x) {
    real += x.real;
    diff += x.diff;
    return *this;
  }
  constexpr FGDualNumber operator-(const FGDualNumber& x) const {
    return FGDualNumber(real - x.real, diff - x.diff);
  }
  FGDualNumber& operator-=(const FGDualNumber& x) {
    real -= x.real;
    diff -= x.diff;
    return *this;
  }
  constexpr FGDualNumber operator*(const FGDualNumber& x) const {
    return FGDualNumber(real * x.real, real * x.diff + diff * x.real);
  }
  constexpr FGDualNumber operator/(const FGDualNumber& x) const {
    double x2 = x.real * x.real;
    return FGDualNumber(real / x.real, (diff * x.real - real * x.diff) / x2);
  }
  FGDualNumber& operator/=(const FGDualNumber& x) {
    double x2 = x.real * x.real;
    diff = (diff * x.real - real * x.diff) / x2;
    real /= x.real;
    return *this;
  }
  constexpr bool operator==(const FGDualNumber& x) const {
    return real == x.real;
  }
  constexpr bool operator!=(const FGDualNumber& x) const {
    return real != x.real;
  }
  constexpr bool operator<=(const FGDualNumber& x) const {
    return real <= x.real;
  }
  constexpr bool operator>=(const FGDualNumber& x) const {
    return real >= x.real;
  }
  constexpr bool operator>(const FGDualNumber& x) const {
    return real > x.real;
  }
  constexpr bool operator<(const FGDualNumber& x) const {
    return real < x.real;
  }
  FGDualNumber& operator*=(const FGDualNumber& x) {
    diff = real * x.diff + diff * x.real;
    real *= x.real;
    return *this;
  }

  /* operators with numeric types */
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  constexpr FGDualNumber operator+(T a) const {
    return FGDualNumber(real + a, diff);
  }
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  constexpr FGDualNumber operator-(T a) const {
    return FGDualNumber(real - a, diff);
  }
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  constexpr FGDualNumber operator*(T a) const {
    return FGDualNumber(real * a, diff * a);
  }
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  FGDualNumber& operator*=(T a) {
    real *= a;
    diff *= a;
    return *this;
  }
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  constexpr FGDualNumber operator/(T a) const {
    return FGDualNumber(real / a, diff / a);
  }
  FGDualNumber operator++(int) {
    FGDualNumber current = *this;
    real++;
    return current;
  }
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  constexpr bool operator==(T a) const {
    return real == a;
  }
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  constexpr bool operator!=(T a) const {
    return real != a;
  }
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  constexpr bool operator<(T a) const {
    return real < a;
  }
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  constexpr bool operator<=(T a) const {
    return real <= a;
  }
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  constexpr bool operator>(T a) const {
    return real > a;
  }
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  constexpr bool operator>=(T a) const {
    return real >= a;
  }
  /* friend functions */
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  friend constexpr FGDualNumber operator/(T a, const FGDualNumber& x) {
    double x2 = x.real * x.real;
    return FGDualNumber(a / x.real, -a * x.diff / x2);
  }
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  friend constexpr FGDualNumber operator-(T a, const FGDualNumber& x) {
    return FGDualNumber(a - x.real, -x.diff);
  }
  template<typename T,
           typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
  friend constexpr FGDualNumber operator+(T a, const FGDualNumber& x) {
    return FGDualNumber(a + x.real, x.diff);
  }
  friend FGDualNumber sqrt(const FGDualNumber& x) {
    double sqr = std::sqrt(x.real);
    return FGDualNumber(sqr, 0.5*x.diff/sqr);
  }
  friend FGDualNumber asin(FGDualNumber x);
  friend FGDualNumber acos(FGDualNumber x);
  friend FGDualNumber atan2(const FGDualNumber& y, const FGDualNumber& x) {
    return FGDualNumber(std::atan2(y.real, x.real),
                        (y.diff * x.real - y.real * x.diff) / (x.real * x.real + y.real * y.real));
  }
  friend FGDualNumber cos(FGDualNumber x);
  friend FGDualNumber sin(FGDualNumber x);
  friend FGDualNumber tan(FGDualNumber x);
  friend FGDualNumber fabs(FGDualNumber x);
  friend FGDualNumber pow(const FGDualNumber& x, double y) {
    return FGDualNumber(std::pow(x.real, y), y * x.diff * std::pow(x.real, y-1.0));
  }
  friend FGDualNumber atan(FGDualNumber x);
  friend FGDualNumber exp(FGDualNumber x);
  friend FGDualNumber log(const FGDualNumber& x) {
    return FGDualNumber(std::log(x.real), x.diff/x.real);
  }
  friend FGDualNumber log10(const FGDualNumber& x) {
    return FGDualNumber(std::log10(x.real), x.diff/(x.real*log(10.)));
  }
  friend FGDualNumber floor(FGDualNumber x);
  friend FGDualNumber ceil(FGDualNumber x);
  friend FGDualNumber modf(FGDualNumber x, FGDualNumber* y);
  friend std::ostream& operator<<(std::ostream& os, const FGDualNumber& x) {
    os << x.real;
    return os;
  }
  friend std::istream& operator>>(std::istream& is, FGDualNumber& x) {
    is >> x.real;
    return is;
  }

private:
  double real;
  double diff;
};

template<typename T,
          typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
inline FGDualNumber max(const FGDualNumber& x, T a) {
  return x >= a ? x : FGDualNumber(a);
}
template<typename T,
          typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
inline FGDualNumber max(T a, const FGDualNumber& x) {
  return x >= a ? x : FGDualNumber(a);
}
template<typename T,
          typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
inline FGDualNumber min(const FGDualNumber& x, T a) {
  return x <= a ? x : FGDualNumber(a);
}
template<typename T,
          typename std::enable_if<std::is_arithmetic<T>::value>::type* =nullptr>
inline FGDualNumber min(T a, const FGDualNumber& x) {
  return x <= a ? x : FGDualNumber(a);
}

inline FGDualNumber exp(FGDualNumber x) {
    double value = std::exp(x.real);
    return FGDualNumber(value, x.diff * value);
}

inline FGDualNumber fabs(FGDualNumber x) {
    if (x.real >= 0.0)
        return FGDualNumber(x.real, x.diff);
    else
        return FGDualNumber(-x.real, -x.diff);
}

inline FGDualNumber cos(FGDualNumber x) {
    return FGDualNumber(std::cos(x.real), -x.diff * std::sin(x.real));
}

inline FGDualNumber sin(FGDualNumber x) {
    return FGDualNumber(std::sin(x.real), x.diff * std::cos(x.real));
}

inline FGDualNumber tan(FGDualNumber x) {
    double value = std::tan(x.real);
    return FGDualNumber(value, x.diff * (1.0 + value * value));
}

inline FGDualNumber asin(FGDualNumber x) {
    return FGDualNumber(std::asin(x.real), x.diff / std::sqrt(1 - x.real * x.real));
}

inline FGDualNumber acos(FGDualNumber x) {
    return FGDualNumber(std::asin(x.real), -x.diff / std::sqrt(1 - x.real * x.real));
}

inline FGDualNumber atan(FGDualNumber x) {
    return FGDualNumber(std::atan(x.real), x.diff / (1.0 + x.real*x.real));
}

inline FGDualNumber floor(FGDualNumber x) {
    // The function floor is constant everywhere but at integer values where
    // its derivative is infinite ?
    // Here, we are assuming that its derivative is zero everywhere.
    return FGDualNumber(std::floor(x.real), 0.);
}
inline FGDualNumber ceil(FGDualNumber x) {
    // The function floor is constant everywhere but at integer values where
    // its derivative is infinite ?
    // Here, we are assuming that its derivative is zero everywhere.
    return FGDualNumber(std::ceil(x.real), 0.);
}

inline FGDualNumber modf(FGDualNumber x, FGDualNumber* y)
{
    double fraction, integer;
    fraction = std::modf(x.real, &integer);
    *y = {integer, 0.};
    return FGDualNumber(fraction, 1.);
}
} // namespace JSBSim

template <class C>
class SGRawValueMethods<C, JSBSim::FGDualNumber> : public SGRawValue<double>
{
public:
  typedef JSBSim::FGDualNumber (C::*getter_t)() const;
  typedef void (C::*setter_t)(JSBSim::FGDualNumber);
  SGRawValueMethods (C &obj, getter_t getter = 0, setter_t setter = 0)
    : _obj(obj), _getter(getter), _setter(setter) {}
  virtual ~SGRawValueMethods () {}
  virtual double getValue () const {
    if (_getter) { return static_cast<double>((_obj.*_getter)()); }
    else { return SGRawValue<double>::DefaultValue(); }
  }
  virtual bool setValue (double value) {
    JSBSim::FGDualNumber v = value;
    if (_setter) { (_obj.*_setter)(v); return true; }
    else return false;
  }
  virtual SGRawValue<double> * clone () const {
    return new SGRawValueMethods<C,JSBSim::FGDualNumber>(_obj, _getter, _setter);
  }
private:
  C &_obj;
  getter_t _getter;
  setter_t _setter;
};

template <>
class SGRawValuePointer<JSBSim::FGDualNumber> : public SGRawValue<double>
{
public:
  SGRawValuePointer (JSBSim::FGDualNumber * ptr) : _ptr(ptr) {}
  virtual ~SGRawValuePointer () {}
  virtual double getValue () const { return static_cast<double>(*_ptr); }
  virtual bool setValue (double value) { *_ptr = JSBSim::FGDualNumber(value); return true; }
  virtual SGRawValue<double> * clone () const {
    return new SGRawValuePointer<JSBSim::FGDualNumber>(_ptr);
  }

private:
  JSBSim::FGDualNumber * _ptr;
};

template <class C>
class SGRawValueMethodsIndexed<C, JSBSim::FGDualNumber> : public SGRawValue<double>
{
public:
  typedef JSBSim::FGDualNumber (C::*getter_t)(int) const;
  typedef void (C::*setter_t)(int, JSBSim::FGDualNumber);
  SGRawValueMethodsIndexed (C &obj, int index,
		     getter_t getter = 0, setter_t setter = 0)
    : _obj(obj), _index(index), _getter(getter), _setter(setter) {}
  virtual ~SGRawValueMethodsIndexed () {}
  virtual double getValue () const {
    if (_getter) { return static_cast<double>((_obj.*_getter)(_index)); }
    else { return SGRawValue<double>::DefaultValue(); }
  }
  virtual bool setValue (double value) {
    JSBSim::FGDualNumber v = value;
    if (_setter) { (_obj.*_setter)(_index, v); return true; }
    else return false;
  }
  virtual SGRawValue<double> * clone () const {
    return new SGRawValueMethodsIndexed<C,JSBSim::FGDualNumber>(_obj, _index, _getter, _setter);
  }
private:
  C &_obj;
  int _index;
  getter_t _getter;
  setter_t _setter;
};

#endif
