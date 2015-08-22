# TestOrbit.py
#
# Check the precision of calculations using the classical body in orbit check
# case.
#
# Copyright (c) 2015 Bertrand Coconnier
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

import sys, unittest, math
import pandas as pd
import numpy as np
from JSBSim_utils import CreateFDM, SandBox, ExecuteUntil


class TestOrbit(unittest.TestCase):
    def setUp(self):
        self.sandbox = SandBox('check_cases', 'orbit')

    # def tearDown(self):
    #     self.sandbox.erase()

    def testOrbit(self):
        fdm = CreateFDM(self.sandbox)
        fdm.load_model('ball')
        fdm.load_ic('reset00_v2', True)
        fdm.set_dt(0.1)
        fdm.set_property_value('simulation/gravity-model', 0)
        fdm.set_property_value('simulation/output/log_rate_hz', 1.)
        fdm.set_property_value('simulation/integrator/rate/translational', 3)
        fdm.set_property_value('simulation/integrator/position/translational', 3)

        slr = fdm.get_property_value('inertial/sea-level-radius_ft')
        R0 = slr + fdm.get_property_value('ic/h-sl-ft')
        RotationRate = 0.00007292115
        GM = 14.0764417572E15
        U0 = math.sqrt(GM / R0) - R0 * RotationRate

        fdm.set_property_value('ic/u-fps', U0)
        fdm.set_property_value('ic/p-rad_sec', 0.0)
        fdm.set_property_value('ic/q-rad_sec', 0.0)
        fdm.set_property_value('ic/r-rad_sec', 0.0)
        fdm.run_ic()

        for i in xrange(10):
            fdm.run()

        self.assertAlmostEqual(fdm.get_property_value('position/epa-rad') / RotationRate,
                               1.0, delta=1E-8)
        self.assertAlmostEqual(fdm.get_property_value('simulation/frame') * 0.1,
                               1.0, delta=1E-8)

        ExecuteUntil(fdm, 10800.)

        results = pd.read_csv(self.sandbox('BallOut.csv'))
        h = results['Altitude ASL (ft)']
        ref = fdm.get_property_value('ic/h-sl-ft') * np.ones((len(h),))
        print (h-ref)[:10]
        # fdm.load_script(self.sandbox.path_to_jsbsim_file('scripts', 'ball_orbit.xml'))
        # fdm.run_ic()

        # while fdm.run():
        #     pass

        # ref, current = Table(), Table()
        # ref.ReadCSV(self.sandbox.elude(self.sandbox.path_to_jsbsim_file('logged_data', 'BallOut.csv')))
        # current.ReadCSV(self.sandbox('BallOut.csv'))

        # diff = ref.compare(current)
        # self.longMessage = True
        # self.assertTrue(diff.empty(), msg='\n'+repr(diff))

suite = unittest.TestLoader().loadTestsFromTestCase(TestOrbit)
test_result = unittest.TextTestRunner(verbosity=2).run(suite)
if test_result.failures or test_result.errors:
    sys.exit(-1)  # 'make test' will report the test failed.
