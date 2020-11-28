// Trivial functions to convert between Real and double.

#include "math/AutomaticDifferentiation.h"

namespace JSBSim {
inline Real DoubleToReal(double x) { return x; }
inline double RealToDouble(const Real& x) { return x; }
};
