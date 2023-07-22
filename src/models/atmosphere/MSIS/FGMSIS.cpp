#include "FC_MSIS.h"

extern "C" {
  void gtd8d(int* iyd, float* sec, float* alt, float* glat,
    float* glong, float* stl, float* f107a, float* f107, float* ap, int* mass,
    float* d, float* t);

  void init(const char* parmpath, const char* parmfile);
}

namespace JSBSim {
class MSIS {
public:
  MSIS() { init(nullptr, "msis21.parm"); }
};
}

int main() {
  JSBSim::MSIS test;
}
