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

import math

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

    def testDeriv_p(self):
        fdm = self.create_fdm()
        fdm.load_model('c172x')
        fdm.load_ic('reset01.xml', True)
        fdm.run_ic()
        while fdm['simulation/sim-time-sec'] < 1.0:
            fdm.run()

        fdm.suspend_integration()
        p_name = 'velocities/p-rad_sec'
        self.assertAlmostEqual(fdm.get_property_gradient(p_name), 0.0)
        p = fdm[p_name]

        fdm.set_property_gradient(p_name, 1.0)
        self.assertAlmostEqual(fdm[p_name], p)
        self.assertAlmostEqual(fdm.get_property_gradient(p_name), 1.0)

        fdm.run()
        self.assertAlmostEqual(fdm[p_name], p)
        self.assertAlmostEqual(fdm.get_property_gradient(p_name), 1.0)

        # Rolling moment (eq D-2 p. 49)
        prop_name = 'aero/coefficient/Clb'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/Clr'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/Clda'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/Cldr'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/Clp'
        self.assertAlmostEqual(fdm[prop_name]/p, fdm.get_property_gradient(prop_name))

        # Pitch moment (eq D-12 p. 50)
        prop_name = 'aero/coefficient/Cmalpha'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CmDf'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/Cmq'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/Cmadot'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/Cmo'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/Cmde'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))

        # Yawing moment (eq D-22 p. 50)
        prop_name = 'aero/coefficient/Cnb'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/Cnr'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/Cnda'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/Cndr'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/Cnp'
        self.assertAlmostEqual(fdm[prop_name]/p, fdm.get_property_gradient(prop_name))

        # Drag force (eq D-32 p. 51)
        prop_name = 'aero/coefficient/CDo'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CDDf'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CDwbh'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CDDe'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CDbeta'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        D_name = 'forces/fwx-aero-lbs'
        self.assertAlmostEqual(fdm.get_property_gradient(D_name), 0.0)

        # Sideforce (eq D-42 p. 51)
        prop_name = 'aero/coefficient/CYb'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CYda'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CYdr'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CYr'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CYp'
        self.assertAlmostEqual(fdm[prop_name]/p, fdm.get_property_gradient(prop_name))
        Y_name = 'forces/fwy-aero-lbs'
        self.assertAlmostEqual(fdm.get_property_gradient(Y_name),
                               fdm.get_property_gradient(prop_name))

        # Lift force (eq D-52 p. 52)
        prop_name = 'aero/coefficient/CLwbh'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CLDf'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CLDe'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CLDadot'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CLq'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        prop_name = 'aero/coefficient/CLalpha'
        self.assertAlmostEqual(0.0, fdm.get_property_gradient(prop_name))
        L_name = 'forces/fwz-aero-lbs'
        self.assertAlmostEqual(fdm.get_property_gradient(L_name),
                               0.0)

        # Roll attitude rate (eq. D-199 p. 62)
        self.assertAlmostEqual(fdm.get_property_gradient('velocities/phidot-rad_sec'), 1.0)

        # Pitch attitude rate (eq. D-214 p. 62)
        self.assertAlmostEqual(fdm.get_property_gradient('velocities/thetadot-rad_sec'), 0.0)

        # Heading attitude rate (eq. D-229 p. 63)
        self.assertAlmostEqual(fdm.get_property_gradient('velocities/psidot-rad_sec'), 0.0)

        # Angle-of-attack rate (eq. D-169 p. 59)
        fdm.run()
        self.assertAlmostEqual(fdm[p_name], p)
        self.assertAlmostEqual(fdm.get_property_gradient(p_name), 1.0)
        alpha = fdm['aero/alpha-rad']
        beta = fdm['aero/beta-rad']
        qS = fdm['aero/qbar-area']
        bi2vel = fdm['aero/bi2vel']
        V = fdm['velocities/vtrue-fps']
        m = fdm['inertia/mass-slugs']
        CLp = 0.0
        adot = -qS*bi2vel*CLp/(V*m*math.cos(beta))-math.tan(beta)*math.cos(alpha)
        self.assertAlmostEqual(fdm.get_property_gradient('aero/alphadot-rad_sec'),
                               adot)

        # Angle-of-sideslip rate (eq. D-184 p. 60)
        CYp = fdm.get_property_gradient('aero/coefficient/CYp')/(qS*bi2vel)
        CDp = 0.0
        bdot = qS*bi2vel/(V*m)*(math.sin(beta)*CDp+math.cos(beta)*CYp)+math.sin(alpha)
        self.assertAlmostEqual(fdm.get_property_gradient('aero/betadot-rad_sec'),
                               bdot)


RunTest(TestGradient)
