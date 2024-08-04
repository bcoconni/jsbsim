# TestHoldDown.py
#
# Test the hold down feature (property forces/hold-down)
#
# Copyright (c) 2015-2024 Bertrand Coconnier
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, see <http://www.gnu.org/licenses/>
#

import math
import unittest

from JSBSim_utils import ExecuteUntil, JSBSimTestCase, RunTest


class TestHoldDown(JSBSimTestCase):
    @unittest.skip("Skip...")
    def test_static_hold_down(self):
        fdm = self.create_fdm()
        fdm.load_model("J246")
        fdm.load_ic("LC39", True)
        fdm["forces/hold-down"] = 1.0
        fdm["ic/phi-deg"] = 0.0
        fdm["ic/psi-true-deg"] = 0.0
        fdm["ic/theta-deg"] = 90.0
        fdm.run_ic()

        pm = fdm.get_property_manager()
        sim_time = pm.get_node("simulation/sim-time-sec")
        R = pm.get_node("position/vrp-radius-ft")
        pi = pm.get_node("velocities/pi-rad_sec")
        qi = pm.get_node("velocities/qi-rad_sec")
        ri = pm.get_node("velocities/ri-rad_sec")
        pidot = pm.get_node("accelerations/pidot-rad_sec2")
        qidot = pm.get_node("accelerations/qidot-rad_sec2")
        ridot = pm.get_node("accelerations/ridot-rad_sec2")
        p = pm.get_node("velocities/p-rad_sec")
        q = pm.get_node("velocities/q-rad_sec")
        r = pm.get_node("velocities/r-rad_sec")
        pdot = pm.get_node("accelerations/pdot-rad_sec2")
        qdot = pm.get_node("accelerations/qdot-rad_sec2")
        rdot = pm.get_node("accelerations/rdot-rad_sec2")
        u = pm.get_node("velocities/u-fps")
        v = pm.get_node("velocities/v-fps")
        w = pm.get_node("velocities/w-fps")
        ui = pm.get_node("velocities/eci-x-fps")
        vi = pm.get_node("velocities/eci-y-fps")
        wi = pm.get_node("velocities/eci-z-fps")
        udot = pm.get_node("accelerations/udot-ft_sec2")
        vdot = pm.get_node("accelerations/vdot-ft_sec2")
        wdot = pm.get_node("accelerations/wdot-ft_sec2")

        R0 = R.get_double_value()
        omega = 0.00007292115
        lat = fdm["position/lat-geod-rad"]
        lat_gc = fdm["position/lat-gc-rad"]
        lon = fdm["position/long-gc-rad"]
        V0 = R0 * math.cos(lat_gc) * omega
        pi_ref = omega * math.sin(lat)
        ri_ref = omega * math.cos(lat)
        t = sim_time.get_double_value()

        while t < 420.0:
            fdm.run()
            t = sim_time.get_double_value()
            self.assertAlmostEqual(R.get_double_value() / R0, 1.0, delta=1e-12)
            self.assertAlmostEqual(pi.get_double_value(), pi_ref, delta=1e-13)
            self.assertAlmostEqual(qi.get_double_value(), 0.0, delta=1e-13)
            self.assertAlmostEqual(ri.get_double_value(), ri_ref, delta=1e-13)
            self.assertAlmostEqual(p.get_double_value(), 0.0, delta=1e-13)
            self.assertAlmostEqual(q.get_double_value(), 0.0, delta=1e-13)
            self.assertAlmostEqual(r.get_double_value(), 0.0, delta=1e-13)
            self.assertAlmostEqual(u.get_double_value(), 0.0, delta=1e-9)
            self.assertAlmostEqual(v.get_double_value(), 0.0, delta=1e-9)
            self.assertAlmostEqual(w.get_double_value(), 0.0, delta=1e-9)
            self.assertAlmostEqual(
                ui.get_double_value() / (-V0 * math.sin(omega * t + lon)),
                1.0,
                delta=1e-5,
            )
            self.assertAlmostEqual(
                vi.get_double_value() / (V0 * math.cos(omega * t + lon)),
                1.0,
                delta=1e-5,
            )
            self.assertAlmostEqual(wi.get_double_value(), 0.0, delta=1e-9)
            self.assertEqual(pdot.get_double_value(), 0.0)
            self.assertEqual(qdot.get_double_value(), 0.0)
            self.assertEqual(rdot.get_double_value(), 0.0)
            self.assertAlmostEqual(pidot.get_double_value(), 0.0, delta=1e-20)
            self.assertAlmostEqual(qidot.get_double_value(), 0.0, delta=1e-20)
            self.assertAlmostEqual(ridot.get_double_value(), 0.0, delta=1e-20)
            self.assertEqual(udot.get_double_value(), 0.0)
            self.assertEqual(vdot.get_double_value(), 0.0)
            self.assertEqual(wdot.get_double_value(), 0.0)

    @unittest.skip("Skip...")
    def test_dynamic_hold_down(self):
        fdm = self.create_fdm()
        fdm.load_model("J246")
        fdm.load_ic("LC39", True)
        fdm["forces/hold-down"] = 1.0
        fdm["ic/phi-deg"] = 0.0
        fdm["ic/psi-true-deg"] = 0.0
        fdm["ic/theta-deg"] = 90.0
        fdm.run_ic()
        # Start solid rocket boosters
        fdm["fcs/throttle-cmd-norm[0]"] = 1.0
        fdm["fcs/throttle-cmd-norm[1]"] = 1.0

        pm = fdm.get_property_manager()
        sim_time = pm.get_node("simulation/sim-time-sec")
        R = pm.get_node("position/vrp-radius-ft")
        pi = pm.get_node("velocities/pi-rad_sec")
        qi = pm.get_node("velocities/qi-rad_sec")
        ri = pm.get_node("velocities/ri-rad_sec")
        pidot = pm.get_node("accelerations/pidot-rad_sec2")
        qidot = pm.get_node("accelerations/qidot-rad_sec2")
        ridot = pm.get_node("accelerations/ridot-rad_sec2")
        p = pm.get_node("velocities/p-rad_sec")
        q = pm.get_node("velocities/q-rad_sec")
        r = pm.get_node("velocities/r-rad_sec")
        pdot = pm.get_node("accelerations/pdot-rad_sec2")
        qdot = pm.get_node("accelerations/qdot-rad_sec2")
        rdot = pm.get_node("accelerations/rdot-rad_sec2")
        udot = pm.get_node("accelerations/udot-ft_sec2")
        vdot = pm.get_node("accelerations/vdot-ft_sec2")
        wdot = pm.get_node("accelerations/wdot-ft_sec2")

        R0 = R.get_double_value()
        omega = 0.00007292115
        lat = fdm["position/lat-geod-rad"]
        pi_ref = omega * math.sin(lat)
        ri_ref = omega * math.cos(lat)

        while sim_time.get_double_value() < 420.0:
            fdm.run()
            self.assertAlmostEqual(R.get_double_value() / R0, 1.0, delta=1e-9)
            self.assertAlmostEqual(pi.get_double_value(), pi_ref, delta=1e-7)
            self.assertAlmostEqual(qi.get_double_value(), 0.0, delta=1e-13)
            self.assertAlmostEqual(ri.get_double_value(), ri_ref, delta=1e-7)
            self.assertAlmostEqual(p.get_double_value(), 0.0, delta=1e-13)
            self.assertAlmostEqual(q.get_double_value(), 0.0, delta=1e-13)
            self.assertAlmostEqual(r.get_double_value(), 0.0, delta=1e-13)
            self.assertEqual(pdot.get_double_value(), 0.0)
            self.assertEqual(qdot.get_double_value(), 0.0)
            self.assertEqual(rdot.get_double_value(), 0.0)
            self.assertAlmostEqual(pidot.get_double_value(), 0.0, delta=1e-20)
            self.assertAlmostEqual(qidot.get_double_value(), 0.0, delta=1e-20)
            self.assertAlmostEqual(ridot.get_double_value(), 0.0, delta=1e-20)
            self.assertEqual(udot.get_double_value(), 0.0)
            self.assertEqual(vdot.get_double_value(), 0.0)
            self.assertEqual(wdot.get_double_value(), 0.0)

    @unittest.skip("Skip...")
    def test_hold_down_with_gnd_reactions(self):
        fdm = self.create_fdm()
        fdm.load_script(self.sandbox.path_to_jsbsim_file("scripts", "c1721.xml"))
        fdm.run_ic()
        ExecuteUntil(fdm, 0.25)
        fdm["forces/hold-down"] = 1.0

        pm = fdm.get_property_manager()
        sim_time = pm.get_node("simulation/sim-time-sec")
        h = pm.get_node("position/h-sl-ft")
        pdot = pm.get_node("accelerations/pdot-rad_sec2")
        qdot = pm.get_node("accelerations/qdot-rad_sec2")
        rdot = pm.get_node("accelerations/rdot-rad_sec2")
        udot = pm.get_node("accelerations/udot-ft_sec2")
        vdot = pm.get_node("accelerations/vdot-ft_sec2")
        wdot = pm.get_node("accelerations/wdot-ft_sec2")

        h0 = h.get_double_value()
        pitch = fdm["attitude/pitch-rad"]
        roll = fdm["attitude/roll-rad"]
        heading = fdm["attitude/heading-true-rad"]

        while sim_time.get_double_value() < 2.0:
            fdm.run()
            self.assertAlmostEqual(h.get_double_value() / h0, 1.0, delta=1e-6)
            self.assertEqual(pdot.get_double_value(), 0.0)
            self.assertEqual(qdot.get_double_value(), 0.0)
            self.assertEqual(rdot.get_double_value(), 0.0)
            self.assertEqual(udot.get_double_value(), 0.0)
            self.assertEqual(vdot.get_double_value(), 0.0)
            self.assertEqual(wdot.get_double_value(), 0.0)

        self.assertAlmostEqual(fdm["attitude/pitch-rad"], pitch)
        self.assertAlmostEqual(fdm["attitude/roll-rad"], roll)
        self.assertAlmostEqual(fdm["attitude/heading-true-rad"], heading)

    def test_spinning_wheel_hold_down(self):
        fdm = self.create_fdm()
        fdm.load_model("ball")
        fdm["forces/hold-down"] = 1.0
        q0 = math.radians(15.0)
        T0 = 2.0 * math.pi / q0
        fdm["ic/phi-deg"] = 0.0
        fdm["ic/theta-deg"] = 0.0
        fdm["ic/psi-true-deg"] = 0.0
        fdm["ic/lat-gc-rad"] = 0.0
        fdm["ic/long-gc-rad"] = 0.0
        fdm["ic/q-rad_sec"] = q0
        fdm.run_ic()

        pm = fdm.get_property_manager()
        sim_time = pm.get_node("simulation/sim-time-sec")
        R = pm.get_node("position/vrp-radius-ft")
        p = pm.get_node("velocities/p-rad_sec")
        q = pm.get_node("velocities/q-rad_sec")
        r = pm.get_node("velocities/r-rad_sec")
        pi = pm.get_node("velocities/pi-rad_sec")
        qi = pm.get_node("velocities/qi-rad_sec")
        ri = pm.get_node("velocities/ri-rad_sec")
        pidot = pm.get_node("accelerations/pidot-rad_sec2")
        qidot = pm.get_node("accelerations/qidot-rad_sec2")
        ridot = pm.get_node("accelerations/ridot-rad_sec2")

        R0 = R.get_double_value()
        omega = 0.00007292115
        t = sim_time.get_double_value()

        while t < 2.0 * T0:
            fdm.run()
            t = sim_time.get_double_value()
            self.assertAlmostEqual(R.get_double_value() / R0, 1.0, delta=1e-9)
            self.assertAlmostEqual(p.get_double_value(), 0.0, delta=1e-7)
            self.assertAlmostEqual(q.get_double_value(), q0, delta=1e-8)
            self.assertAlmostEqual(r.get_double_value(), 0.0, delta=1e-6)
            # self.assertAlmostEqual(pi.get_double_value(), omega*math.cos(omega*t), delta=1e-6)
            self.assertAlmostEqual(qi.get_double_value(), q0*math.cos(omega*t), delta=1e-4)
            self.assertAlmostEqual(ri.get_double_value(), q0*math.sin(omega*t), delta=1e-2)
            # self.assertAlmostEqual(qidot.get_double_value(), -q0*omega*math.sin(omega*t), delta=1e-4)
            self.assertAlmostEqual(ridot.get_double_value(),(q0*omega*math.cos(omega*t)),  delta=1e-10)


RunTest(TestHoldDown)
