#include <cxxtest/TestSuite.h>

#include <FGFDMExec.h>
#include <models/FGInertial.h>
#include "TestAssertions.h"

using namespace JSBSim;

const Real epsilon = 1e-5;
constexpr Real degtorad = M_PI / 180.;

class FGInertialTest : public CxxTest::TestSuite
{
public:
  void testTransformationMatricesSphericalEarth() {
    FGFDMExec fdmex;
    auto planet = fdmex.GetInertial();
    fdmex.SetPropertyValue("simulation/gravity-model", 0);
    FGLocation loc;
    FGMatrix33 Tec2l;
    Real radius = planet->GetSemimajor();

    for(Real lon=-180.; lon <= 180.; lon += 30.) {
      Real longitude = lon * degtorad;
      Real cosLon = cos(longitude);
      Real sinLon = sin(longitude);

      for(Real lat=-90.; lat <= 90.; lat += 30.) {
        Real latitude = lat * degtorad;
        loc.SetPosition(longitude, latitude, radius);
        Real cosLat = cos(latitude);
        Real sinLat = sin(latitude);
        Tec2l = { -cosLon*sinLat, -sinLon*sinLat,  cosLat,
                      -sinLon   ,     cosLon    ,    0.0 ,
                  -cosLon*cosLat, -sinLon*cosLat, -sinLat  };
        TS_ASSERT_MATRIX_EQUALS(planet->GetTec2l(loc), Tec2l);
        TS_ASSERT_MATRIX_EQUALS(planet->GetTl2ec(loc), Tec2l.Transposed());
      }
    }
  }

  void testTransformationMatricesWGS84Earth() {
    FGFDMExec fdmex;
    auto planet = fdmex.GetInertial();
    fdmex.SetPropertyValue("simulation/gravity-model", 1);
    FGLocation loc;
    FGMatrix33 Tec2l;

    loc.SetEllipse(planet->GetSemimajor(), planet->GetSemiminor());

    for(Real lat=-90.; lat <= 90.; lat += 30.) {
      Real latitude = lat * degtorad;
      Real cosLat = cos(latitude);
      Real sinLat = sin(latitude);

      for(Real lon=-180.; lon <= 180.; lon += 30.) {
        Real longitude = lon * degtorad;
        loc.SetPositionGeodetic(longitude, latitude, 0.0);
        Real cosLon = cos(longitude);
        Real sinLon = sin(longitude);
        Tec2l = { -cosLon*sinLat, -sinLon*sinLat,  cosLat,
                      -sinLon   ,     cosLon    ,    0.0 ,
                  -cosLon*cosLat, -sinLon*cosLat, -sinLat  };
        TS_ASSERT_MATRIX_EQUALS(planet->GetTec2l(loc), Tec2l);
        TS_ASSERT_MATRIX_EQUALS(planet->GetTl2ec(loc), Tec2l.Transposed());
      }
    }
  }
};
