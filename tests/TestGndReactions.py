# TestGndReactions.py
#
# Checks that ground reactions are working
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

import unittest, sys
from JSBSim_utils import SandBox, CreateFDM


class TestGndReactions(unittest.TestCase):
    def setUp(self):
        self.sandbox = SandBox()

    def tearDown(self):
        self.sandbox.erase()

    def test_ground_reactions(self):
        fdm = CreateFDM(self.sandbox)
        fdm.set_aircraft_path(self.sandbox.path_to_jsbsim_file('tests'))
        fdm.load_model('tripod', False)
        fdm.set_property_value('ic/h-sl-ft', 0.0)
        fdm.set_property_value('simulation/integrator/rate/rotational', 0)
        fdm.set_property_value('simulation/integrator/rate/translational', 1)
        fdm.set_property_value('simulation/integrator/position/rotational', 0)
        fdm.set_property_value('simulation/integrator/position/translational', 1)

        fdm.run_ic()
        fdm.do_trim(2)
        print "t=%f" % (fdm.get_property_value('simulation/sim-time-sec'),)
        print fdm.get_property_value('contact/unit[0]/compression-ft')*10000, \
            fdm.get_property_value('contact/unit[1]/compression-ft')*10000, \
            fdm.get_property_value('contact/unit[2]/compression-ft')*10000
        print fdm.get_property_value('position/h-sl-ft')
        print fdm.get_property_value('forces/fbx-gear-lbs'), \
            fdm.get_property_value('forces/fby-gear-lbs'), \
            fdm.get_property_value('forces/fbz-gear-lbs')
        print fdm.get_property_value('forces/fbx-total-lbs'), \
            fdm.get_property_value('forces/fby-total-lbs'), \
            fdm.get_property_value('forces/fbz-total-lbs')
        print fdm.get_property_value('accelerations/udot-ft_sec2'), \
            fdm.get_property_value('accelerations/vdot-ft_sec2'), \
            fdm.get_property_value('accelerations/wdot-ft_sec2')

        fdm.run()
        print "t=%f" % (fdm.get_property_value('simulation/sim-time-sec'),)
        print fdm.get_property_value('contact/unit[0]/compression-ft')*10000, \
            fdm.get_property_value('contact/unit[1]/compression-ft')*10000, \
            fdm.get_property_value('contact/unit[2]/compression-ft')*10000
        print fdm.get_property_value('position/h-sl-ft')
        print fdm.get_property_value('forces/fbx-gear-lbs'), \
            fdm.get_property_value('forces/fby-gear-lbs'), \
            fdm.get_property_value('forces/fbz-gear-lbs')
        print fdm.get_property_value('forces/fbx-total-lbs'), \
            fdm.get_property_value('forces/fby-total-lbs'), \
            fdm.get_property_value('forces/fbz-total-lbs')
        print fdm.get_property_value('accelerations/udot-ft_sec2'), \
            fdm.get_property_value('accelerations/vdot-ft_sec2'), \
            fdm.get_property_value('accelerations/wdot-ft_sec2')

        fdm.run()
        print "t=%f" % (fdm.get_property_value('simulation/sim-time-sec'),)
        print fdm.get_property_value('contact/unit[0]/compression-ft')*10000, \
            fdm.get_property_value('contact/unit[1]/compression-ft')*10000, \
            fdm.get_property_value('contact/unit[2]/compression-ft')*10000
        print fdm.get_property_value('position/h-sl-ft')
        print fdm.get_property_value('forces/fbx-gear-lbs'), \
            fdm.get_property_value('forces/fby-gear-lbs'), \
            fdm.get_property_value('forces/fbz-gear-lbs')
        print fdm.get_property_value('forces/fbx-total-lbs'), \
            fdm.get_property_value('forces/fby-total-lbs'), \
            fdm.get_property_value('forces/fbz-total-lbs')
        print fdm.get_property_value('accelerations/udot-ft_sec2'), \
            fdm.get_property_value('accelerations/vdot-ft_sec2'), \
            fdm.get_property_value('accelerations/wdot-ft_sec2')

        fdm.run()
        print "t=%f" % (fdm.get_property_value('simulation/sim-time-sec'),)
        print fdm.get_property_value('contact/unit[0]/compression-ft')*10000, \
            fdm.get_property_value('contact/unit[1]/compression-ft')*10000, \
            fdm.get_property_value('contact/unit[2]/compression-ft')*10000
        print fdm.get_property_value('position/h-sl-ft')
        print fdm.get_property_value('forces/fbx-gear-lbs'), \
            fdm.get_property_value('forces/fby-gear-lbs'), \
            fdm.get_property_value('forces/fbz-gear-lbs')
        print fdm.get_property_value('forces/fbx-total-lbs'), \
            fdm.get_property_value('forces/fby-total-lbs'), \
            fdm.get_property_value('forces/fbz-total-lbs')
        print fdm.get_property_value('accelerations/udot-ft_sec2'), \
            fdm.get_property_value('accelerations/vdot-ft_sec2'), \
            fdm.get_property_value('accelerations/wdot-ft_sec2')

        fdm.run()
        print "t=%f" % (fdm.get_property_value('simulation/sim-time-sec'),)
        print fdm.get_property_value('contact/unit[0]/compression-ft')*10000, \
            fdm.get_property_value('contact/unit[1]/compression-ft')*10000, \
            fdm.get_property_value('contact/unit[2]/compression-ft')*10000
        print fdm.get_property_value('position/h-sl-ft')
        print fdm.get_property_value('forces/fbx-gear-lbs'), \
            fdm.get_property_value('forces/fby-gear-lbs'), \
            fdm.get_property_value('forces/fbz-gear-lbs')
        print fdm.get_property_value('forces/fbx-total-lbs'), \
            fdm.get_property_value('forces/fby-total-lbs'), \
            fdm.get_property_value('forces/fbz-total-lbs')
        print fdm.get_property_value('accelerations/udot-ft_sec2'), \
            fdm.get_property_value('accelerations/vdot-ft_sec2'), \
            fdm.get_property_value('accelerations/wdot-ft_sec2')

suite = unittest.TestLoader().loadTestsFromTestCase(TestGndReactions)
test_result = unittest.TextTestRunner(verbosity=2).run(suite)
if test_result.failures or test_result.errors:
    sys.exit(-1) # 'make test' will report the test failed.
