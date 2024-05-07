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

#include <memory>

#include "math/FGLocation.h"
#include "_jsbsim.h"
#include "PyGroundCallback.h"

namespace JSBSim {

PyObject* FGGroundCallbackClass = nullptr;

PyGroundCallback::PyGroundCallback(PyObject* callback_pyclass)
  : groundcallback_pyclass(nullptr)
{
  if (PyObject_IsInstance(callback_pyclass, FGGroundCallbackClass)) {
    groundcallback_pyclass = callback_pyclass;
    Py_INCREF(groundcallback_pyclass);
  }
  else {
    PyErr_SetString(PyExc_TypeError, "Expected a FGGroundCallback instance");
  }
}
double PyGroundCallback::GetAGLevel (double t, const FGLocation& location,
                          FGLocation& contact,
                          FGColumnVector3& normal, FGColumnVector3& v,
                          FGColumnVector3& w) const
{
  PyObject *obj = PyObject_CallObject((PyObject *) &PyFGLocation_t, nullptr);
  if (obj) {
    struct PyFGLocation *l = (struct PyFGLocation *) obj;
    l->thisptr = std::make_shared<FGLocation>(location);

    PyObject* pyresult = PyObject_CallMethod(groundcallback_pyclass, "get_agl_level", "do", t, obj);
    Py_DECREF(obj);
    if (pyresult) {
      double result = PyFloat_AsDouble(pyresult);
      Py_DECREF(pyresult);
      if (!PyErr_Occurred())
        return result;
    }
  }
  return 0.0;
}

void PyGroundCallback::SetTime(double t)
{
  PyObject* pyresult = PyObject_CallMethod(groundcallback_pyclass, "set_time", "d", t);
  if (pyresult)
    this->time = t;
  else {
    PyErr_Print();
    PyErr_Clear();
  }
  Py_XDECREF(pyresult);
}
}
