/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Header: FGColumnVector3.h
Author: Originally by Tony Peden [formatted and adapted here by Jon Berndt]
Date started: Unknown

 ------ Copyright (C) 2001 by Tony Peden and Jon S. Berndt (jon@jsbsim.org)

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
??/??/???? ??   Initial version and more.
03/06/2004 MF   Rework, document and do much inlineing.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef FGCOLUMNVECTOR3_H
#define FGCOLUMNVECTOR3_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <iosfwd>
#include <string>
#include <string.h>

#include "JSBSim_API.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace JSBSim {

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** This class implements a 3 element column vector.
    @author Jon S. Berndt, Tony Peden, et. al.
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class JSBSIM_API FGColumnVector3
{
public:
  /** Default initializer.
      Create a zero vector.   */
  FGColumnVector3(void) noexcept {
    memset(data, 0, sizeof(data));
  }

  /** Initialization by given values.
      @param X value of the x-conponent.
      @param Y value of the y-conponent.
      @param Z value of the z-conponent.
      Create a vector from the doubles given in the arguments.   */
  FGColumnVector3(const double X, const double Y, const double Z) noexcept {
    data[1] = X;
    data[2] = Y;
    data[3] = Z;
  }

  /** Copy constructor.
      @param v Vector which is used for initialization.
      Create copy of the vector given in the argument.   */
  FGColumnVector3(const FGColumnVector3& v) noexcept {
    memcpy(data, v.data, sizeof(data));
  }

  /** Read access the entries of the vector.
      @param idx the component index.
      Return the value of the matrix entry at the given index.
      Indices are counted starting with 1.
      Note that the index given in the argument is unchecked.   */
  double operator()(const unsigned int idx) const noexcept { return data[idx]; }

  /** Write access the entries of the vector.
      @param idx the component index.
      Return a reference to the vector entry at the given index.
      Indices are counted starting with 1.
      Note that the index given in the argument is unchecked.   */
  double& operator()(const unsigned int idx) noexcept { return data[idx]; }

  /** Read access the entries of the vector.
      @param idx the component index.
      Return the value of the matrix entry at the given index.
      Indices are counted starting with 1.
      This function is just a shortcut for the <tt>double
      operator()(unsigned int idx) const</tt> function. It is
      used internally to access the elements in a more convenient way.
      Note that the index given in the argument is unchecked.   */
  double Entry(const unsigned int idx) const noexcept { return data[idx]; }

  /** Write access the entries of the vector.
      @param idx the component index.
      Return a reference to the vector entry at the given index.
      Indices are counted starting with 1.
      This function is just a shortcut for the <tt>double&
      operator()(unsigned int idx)</tt> function. It is
      used internally to access the elements in a more convenient way.
      Note that the index given in the argument is unchecked.   */
  double& Entry(const unsigned int idx) noexcept { return data[idx]; }

  /** Prints the contents of the vector
      @param delimeter the item separator (tab or comma)
      @return a string with the delimeter-separated contents of the vector  */
  std::string Dump(const std::string& delimeter) const;

  /** Assignment operator.
      @param b source vector.
      Copy the content of the vector given in the argument into *this.   */
  FGColumnVector3& operator=(const FGColumnVector3& v) noexcept {
    memcpy(data, v.data, sizeof(data));
    return *this;
  }

  /** Assignment operator.
      @param lv initializer list of at most 3 values (i.e. {x, y, Z})
      Copy the content of the list into *this. */
  FGColumnVector3& operator=(std::initializer_list<double> lv) noexcept {
    double *v = data;
    for(auto &x : lv)
      *(++v) = x;

    return *this;
  }

  /**  Comparison operator.
      @param b other vector.
      Returns true if both vectors are exactly the same.   */
  bool operator==(const FGColumnVector3& b) const noexcept {
    return data[1] == b.data[1] && data[2] == b.data[2] && data[3] == b.data[3];
  }

  /** Comparison operator.
      @param b other vector.
      Returns false if both vectors are exactly the same.   */
  bool operator!=(const FGColumnVector3& b) const noexcept { return ! operator==(b); }

  /** Multiplication by a scalar.
      @param scalar scalar value to multiply the vector with.
      @return The resulting vector from the multiplication with that scalar.
      Multiply the vector with the scalar given in the argument.   */
  FGColumnVector3 operator*(const double scalar) const noexcept {
    return FGColumnVector3(scalar*data[1], scalar*data[2], scalar*data[3]);
  }

  /** Multiply by 1/scalar.
      @param scalar scalar value to devide the vector through.
      @return The resulting vector from the division through that scalar.
      Multiply the vector with the 1/scalar given in the argument.   */
  FGColumnVector3 operator/(const double scalar) const;

  /** Cross product multiplication.
      @param V vector to multiply with.
      @return The resulting vector from the cross product multiplication.
      Compute and return the cross product of the current vector with
      the given argument.   */
  FGColumnVector3 operator*(const FGColumnVector3& V) const noexcept {
    return FGColumnVector3( data[2] * V.data[3] - data[3] * V.data[2],
                            data[3] * V.data[1] - data[1] * V.data[3],
                            data[1] * V.data[2] - data[2] * V.data[1] );
  }

  /// Addition operator.
  FGColumnVector3 operator+(const FGColumnVector3& B) const noexcept {
    return FGColumnVector3( data[1] + B.data[1], data[2] + B.data[2],
                            data[3] + B.data[3] );
  }

  /// Subtraction operator.
  FGColumnVector3 operator-(const FGColumnVector3& B) const noexcept {
    return FGColumnVector3( data[1] - B.data[1], data[2] - B.data[2],
                            data[3] - B.data[3] );
  }

  /// Subtract an other vector.
  FGColumnVector3& operator-=(const FGColumnVector3 &B) noexcept {
    data[1] -= B.data[1];
    data[2] -= B.data[2];
    data[3] -= B.data[3];
    return *this;
  }

  /// Add an other vector.
  FGColumnVector3& operator+=(const FGColumnVector3 &B) noexcept {
    data[1] += B.data[1];
    data[2] += B.data[2];
    data[3] += B.data[3];
    return *this;
  }

  /// Scale by a scalar.
  FGColumnVector3& operator*=(const double scalar) noexcept {
    data[1] *= scalar;
    data[2] *= scalar;
    data[3] *= scalar;
    return *this;
  }

  /// Scale by a 1/scalar.
  FGColumnVector3& operator/=(const double scalar);

  void InitMatrix(void) noexcept { memset(data, 0, sizeof(data)); }
  void InitMatrix(const double a) noexcept { data[1] = data[2] = data[3] = a; }
  void InitMatrix(const double a, const double b, const double c) noexcept {
    data[1]=a; data[2]=b; data[3]=c;
  }

  /** Length of the vector.
      Compute and return the euclidean norm of this vector.   */
  double Magnitude(void) const;

  /** Length of the vector in a coordinate axis plane.
      Compute and return the euclidean norm of this vector projected into
      the coordinate axis plane idx1-idx2.   */
  double Magnitude(const int idx1, const int idx2) const;

  /** Normalize.
      Normalize the vector to have the Magnitude() == 1.0. If the vector
      is equal to zero it is left untouched.   */
  FGColumnVector3& Normalize(void);

private:
  double data[4];
};

/** Dot product of two vectors
    Compute and return the euclidean dot (or scalar) product of two vectors
    v1 and v2 */
inline double DotProduct(const FGColumnVector3& v1, const FGColumnVector3& v2) noexcept {
  return v1(1)*v2(1) + v1(2)*v2(2) + v1(3)*v2(3);
}

/** Scalar multiplication.
    @param scalar scalar value to multiply with.
    @param A Vector to multiply.
    Multiply the Vector with a scalar value. Note: At this time, this
    operator MUST be inlined, or a multiple definition link error will occur.*/
inline FGColumnVector3 operator*(double scalar, const FGColumnVector3& A) noexcept {
  // use already defined operation.
  return A*scalar;
}

/** Write vector to a stream.
    @param os Stream to write to.
    @param col vector to write.
    Write the vector to a stream.*/
JSBSIM_API std::ostream& operator<<(std::ostream& os, const FGColumnVector3& col);

} // namespace JSBSim

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#endif
