#ifndef AUTOMATIC_DIFFERENTIATION_H
#define AUTOMATIC_DIFFERENTIATION_H

#ifdef AUTOMATIC_DIFFERENTIATION
#include "DualNumber.h"
using Real = JSBSim::FGDualNumber;
#else
using Real = double;
#endif

#endif // AUTOMATIC_DIFFERENTIATION_H
