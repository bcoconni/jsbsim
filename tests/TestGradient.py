# TestGradient.py
#
# A test that checks the automatic differentiation feature.
#
# Copyright (c) 2021 Bertrand Coconnier
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

from JSBSim_utils import JSBSimTestCase, RunTest, jsbsim


class TestGradient(JSBSimTestCase):
    def testBasic(self):
        fdm = self.create_fdm()
        fdm.load_model('c172x')
        fdm.load_ic('reset01.xml', True)
        fdm.run_ic()
        fdm.run()

        jsbsim.FGJSBBase().debug_lvl = 1
        fdm['simulation/do_simple_trim'] = 1
        jsbsim.FGJSBBase().debug_lvl = 0
        fdm.suspend_integration()
        self.assertAlmostEqual(fdm.get_property_gradient('velocities/u-fps'), 0.0)
        u = fdm['velocities/u-fps']

        fdm.set_property_gradient('velocities/u-fps', 1.0)
        self.assertAlmostEqual(fdm['velocities/u-fps'], u)
        self.assertAlmostEqual(fdm.get_property_gradient('velocities/u-fps'), 1.0)

        fdm.run()
        self.assertAlmostEqual(fdm['velocities/u-fps'], u)
        rho = fdm['atmosphere/rho-slugs_ft3']
        self.assertAlmostEqual(fdm.get_property_gradient('aero/qbar-psf'), rho*u)


RunTest(TestGradient)
