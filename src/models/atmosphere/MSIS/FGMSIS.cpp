#include "FC_MSIS.h"

extern "C" {
  void gtd8d(int* iyd, float* sec, float* alt, float* glat,
    float* glong, float* stl, float* f107a, float* f107, float* ap, int* mass,
    float* d, float* t);

  void msis_init_msisinit(const char* parmpath, const char* parmfile, int* iun,
    bool* switch_gfn, float* switch_legacy, bool* lzalt_type, bool* lspec_select,
    bool* lmass_include, bool* lN2_msis00);
}

namespace JSBSim {
class MSIS {
public:
  MSIS() {
    msis_init_msisinit(nullptr, "msis20.parm", nullptr, nullptr, nullptr, nullptr,
                        nullptr, nullptr, nullptr);
  }
};
}

int main() {
  JSBSim::MSIS test;
}
