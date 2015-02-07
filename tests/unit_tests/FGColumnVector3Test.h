#include <cxxtest/TestSuite.h>
#include <math/FGColumnVector3.h>

class FGColumnVector3Test : public CxxTest::TestSuite
{
public:
  void testNullity(void) {
    const JSBSim::FGColumnVector3 v0;

    TS_ASSERT_EQUALS(v0.Entry(1), 0.0);
    TS_ASSERT_EQUALS(v0.Entry(2), 0.0);
    TS_ASSERT_EQUALS(v0.Entry(3), 0.0);
  }


  void testConstructorWithLitterals(void) {
    JSBSim::FGColumnVector3 v1(1., 0., -2.);

    TS_ASSERT_EQUALS(v1.Entry(1), 1.0);
    TS_ASSERT_EQUALS(v1.Entry(2), 0.0);
    TS_ASSERT_EQUALS(v1.Entry(3), -2.0);
  }

  void testCopyConstructor(void) {
    JSBSim::FGColumnVector3 v1(1., 0., -2.);
    JSBSim::FGColumnVector3 v2(v1);

    // First make sure that v1 and v2 are identical
    TS_ASSERT_EQUALS(v2.Entry(1), v1.Entry(1));
    TS_ASSERT_EQUALS(v2.Entry(2), v1.Entry(2));
    TS_ASSERT_EQUALS(v2.Entry(3), v1.Entry(3));

    // Second, make sure that v1 and v2 are distinct copies
    // i.e. that v1 and v2 does not point to the same memory location
    v1.Entry(2) = 5.0;
    TS_ASSERT_EQUALS(v2.Entry(2), 0.0); // v2[2] must remain unchanged
    TS_ASSERT_EQUALS(v1.Entry(2), 5.0); // v1[2] must now contain 5.0
  }

  void testAssignment(void) {
    JSBSim::FGColumnVector3 v;
    JSBSim::FGColumnVector3 v1(1., 0., -2.);

    for (unsigned int i=1; i < 4; i++) {
      TS_ASSERT_EQUALS(v(i), 0.0);
      TS_ASSERT_EQUALS(v.Entry(i), 0.0);
    }

    v = v1;

    TS_ASSERT_EQUALS(v1.Entry(1), 1.0);
    TS_ASSERT_EQUALS(v1.Entry(2), 0.0);
    TS_ASSERT_EQUALS(v1.Entry(3), -2.0);

    // Make sure that v and v1 are distinct copies
    double x = v(2);
    TS_ASSERT_EQUALS(x, v1.Entry(2));
    v1.Entry(2) = -8.0;
    TS_ASSERT_EQUALS(v(2), x);
    TS_ASSERT_EQUALS(v.Entry(2), x);

    for (unsigned int i=1; i < 4; i++) {
      v = v1;
      x = v1.Entry(i) + 1.0;
      v.Entry(i) = x;
      for (unsigned int j=1; j < 4; j++) {
        if (i == j) {
          TS_ASSERT_EQUALS(v(i), x);
          TS_ASSERT_EQUALS(v.Entry(i), x);
        }
        else {
          TS_ASSERT_EQUALS(v(j), v1.Entry(j));
          TS_ASSERT_EQUALS(v.Entry(j), v1.Entry(j));
        }
      }
    }
  }

  void testEquality() {
    JSBSim::FGColumnVector3 v;
    JSBSim::FGColumnVector3 v1(1., 0., -2.);

    v = v1;
    TS_ASSERT(v == v1);

    for (unsigned int i=1; i < 4; i++) {
      v = v1;
      v(i) = v1.Entry(i) + 1.0;
      TS_ASSERT(v != v1);

      for (unsigned int j=1; j < 4; j++) {
        if (i == j)
          v(i) = v1.Entry(i);
        else
          v(j) = v1.Entry(j) + 1.0;
      }

      TS_ASSERT(v != v1);
    }
  }

  void testInitMatrix() {
    JSBSim::FGColumnVector3 v0;
    JSBSim::FGColumnVector3 v1(1., 0., -2.);

    v1.InitMatrix();
    TS_ASSERT(v1 == v0);

    v1.InitMatrix(1.0);
    TS_ASSERT_EQUALS(v1(1), 1.0);
    TS_ASSERT_EQUALS(v1(2), 1.0);
    TS_ASSERT_EQUALS(v1(3), 1.0);

    v1.InitMatrix(-1.0, 2.0, 0.0);
    TS_ASSERT_EQUALS(v1(1), -1.0);
    TS_ASSERT_EQUALS(v1(2), 2.0);
    TS_ASSERT_EQUALS(v1(3), 0.0);
  }

  void testOperations() {
    JSBSim::FGColumnVector3 v0;
    JSBSim::FGColumnVector3 v1(1., 0., -2.);
    JSBSim::FGColumnVector3 v_twice = 2.0 * v1;

    TS_ASSERT_EQUALS(v_twice(1), 2.0 * v1(1));
    TS_ASSERT_EQUALS(v_twice(2), 2.0 * v1(2));
    TS_ASSERT_EQUALS(v_twice(3), 2.0 * v1(3));
    TS_ASSERT(v_twice == v1 * 2.0);
    TS_ASSERT(v_twice / 2.0 == v1);
    TS_ASSERT(v_twice / 0.0 == v0);
    TS_ASSERT(v_twice + v1 == 3.0 * v1);
    TS_ASSERT(v_twice - v1 == v1);
    v_twice /= 2.0;
    TS_ASSERT(v_twice == v1);
    v_twice.InitMatrix(1.0);
    v_twice *= -1.0;
    TS_ASSERT_EQUALS(v_twice(1), -1.0);
    TS_ASSERT_EQUALS(v_twice(2), -1.0);
    TS_ASSERT_EQUALS(v_twice(3), -1.0);

    JSBSim::FGColumnVector3 v(1.0, -2.0, 0.0);
    v += v_twice;
    TS_ASSERT_EQUALS(v(1), 0.0);
    TS_ASSERT_EQUALS(v(2), -3.0);
    TS_ASSERT_EQUALS(v(3), -1.0);

    v -= v_twice;
    TS_ASSERT_EQUALS(v(1), 1.0);
    TS_ASSERT_EQUALS(v(2), -2.0);
    TS_ASSERT_EQUALS(v(3), 0.0);

    v.InitMatrix(1.0, -2.0, 0.5);
    TS_ASSERT_EQUALS(DotProduct(v,v), 5.25);
    v /= 0.0;
    TS_ASSERT_EQUALS(v(1), 1.0);
    TS_ASSERT_EQUALS(v(2), -2.0);
    TS_ASSERT_EQUALS(v(3), 0.5);
  }

  void testCrossProduct() {
    JSBSim::FGColumnVector3 X(1.0,0.0,0.0);
    JSBSim::FGColumnVector3 Y(0.0,1.0,0.0);
    JSBSim::FGColumnVector3 Z(0.0,0.0,1.0);

    TS_ASSERT_EQUALS(X * Y, Z);
    TS_ASSERT_EQUALS(Y * X, -1.0 * Z);
    TS_ASSERT_EQUALS(Y * Z, X);
    TS_ASSERT_EQUALS(Z * Y, -1.0 * X);
    TS_ASSERT_EQUALS(Z * X, Y);
    TS_ASSERT_EQUALS(X * Z, -1.0 * Y);
    TS_ASSERT_EQUALS(DotProduct(X, Y), 0.0);
    TS_ASSERT_EQUALS(DotProduct(Y, Z), 0.0);
    TS_ASSERT_EQUALS(DotProduct(X, Z), 0.0);
}

  void testNorm() {
    JSBSim::FGColumnVector3 v0;
    JSBSim::FGColumnVector3 v(3.0, 4.0, 0.0);

    TS_ASSERT_EQUALS(v0.Magnitude(), 0.0);
    TS_ASSERT_EQUALS(v0.Normalize(), v0);
    TS_ASSERT_EQUALS(v.Magnitude(1,3), 3.0);
    TS_ASSERT_EQUALS(v.Magnitude(2,3), 4.0);
    TS_ASSERT_EQUALS(v.Magnitude(1,2), 5.0);
    TS_ASSERT_EQUALS(v.Magnitude(), 5.0);
    TS_ASSERT_EQUALS(DotProduct(v,v), v.Magnitude() * v.Magnitude());

    v.Normalize();
    TS_ASSERT_DELTA(v(1), 0.6, 1E-9);
    TS_ASSERT_DELTA(v(2), 0.8, 1E-9);
    TS_ASSERT_EQUALS(v(3), 0.0);
  }

  void testOutput() {
    JSBSim::FGColumnVector3 v1(1., 0., -2.);
    std::string s = v1.Dump(" , ");

    TS_ASSERT_EQUALS(s, std::string("1 , 0 , -2"));
    std::ostringstream os;
    os << v1;
    TS_ASSERT_EQUALS(os.str(), std::string("1 , 0 , -2"));
  }
};
