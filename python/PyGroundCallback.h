/* Ground callback class for Python
 *
 * Copyright (c) 2024 Bertrand Coconnier
 *
* This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
  */


#include "input_output/FGGroundCallback.h"

#ifndef PYGROUNDCALLBACK_H
#define PYGROUNDCALLBACK_H

namespace JSBSim {

extern PyObject* FGGroundCallbackClass;

class PyGroundCallback : public FGGroundCallback
{
public:
  explicit PyGroundCallback(PyObject* callback_pyclass);
  ~PyGroundCallback() override { Py_XDECREF(groundcallback_pyclass); }

  double GetAGLevel (double t, const FGLocation& location,
                            FGLocation& contact,
                            FGColumnVector3& normal, FGColumnVector3& v,
                            FGColumnVector3& w) const override;

  void SetTime(double t) override;
private:
  PyObject* groundcallback_pyclass;
};
} // namespace JSBSim
#endif // PYGROUNDCALLBACK_H
