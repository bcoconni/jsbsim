# Falcon 9 flight dynamics model

## Introduction

The purpose of this repository is to build a flight dynamics model (FDM) of the [Falcon 9 rocket from SpaceX](https://www.spacex.com/vehicles/falcon-9) with a particular focus on the return of the 1st stage to the ground.

For that purpose we will use [JSBSim](https://github.com/JSBSim-Team/jsbsim), a general purpose flight dynamics model software. JSBSim will be used via its Python module as this allows to use the powerful Jupyter notebooks to document the results, plot curves and insert comments.

The model will be built from the ground up and each stage of the process will be explained in details in order to familiarize the reader with JSBSim. It will be assumed that the user knows the basics of the [XML markup language](https://en.wikipedia.org/wiki/XML) which is the format in which the FDM is described in JSBSim.

### Disclaimer

The purpose of this repository is educational and the model it provides is not endorsed by SpaceX or any of its affiliates. The model is built from publicly available data and is by no means intended to be a high fidelity reproduction of the real Falcon 9 rocket flight dynamics performance.

## Model description

JSBSim uses the XML markup language to describe a model. For a rocket we will need 5 files:

1. The FDM description which will be stored in the file `aircraft/Falcon9/Falcon9.xml`
2. A generic description of the rocket engine. The Falcon 9 rocket has 9 identical engines on its first stage and their common features will be described in the file `engine/Merlin_1-D.xml`.
3. A generic description of the engine nozzle. As for the engines, the nozzles of the 9 first stage engines are identical and their common features will be described in the file `engine/Merlin_1-D_nozzle.xml`.
4. The rocket initial conditions are described in the file `aircraft/Falcon9.xml/LC39.xml` which specifies the location and orientation at which JSBSim will spawn the rocket. The file will basically contain the coordinate of the KSC Launch Complex 39 (hence the file name `LC39.xml`) and make sure the rocket is orientated upwards.
5. A script which describes the launch sequence:
    - Place the rocket `Falcon9.xml` at the launch pad `LC39.xml`
    - Ignite the engines
    - Specify some simulation parameters if needed (i.e. time step, integration algorithms, etc.)
    - Store the flight parameters in a CSV file
    - etc.

The files tree is then:

```bash
aircraft/
    Falcon9/
        Falcon9.xml
        LC39.xml
engine/
   Merlin_1-D.xml
   Merlin_1-D_nozzle.xml
scripts/
   Falcon9_static_firing.xml
```

The first stage to build a new flight dynamics model is to gather data. Fortunately there is much data available on the internet.

We will use data for the Falcon 9 block 5.

The longitudinal axis `X` is oriented upward with the reference `X=0` at the launch table. Axes `Y` and `Z` are perpendicular to `X` i.e. parallel to the ground at the launch site. Their exact orientation beyond their perpendicularity to `X` is irrelevant since the rocket is symmetric around its longitudinal axis.

To start with a simple model, we will neglect the effect of the aerodynamics and ground reactions will be replaced by the hold down feature of JSBSim. The hold down model is basically a means to force a vehicle to remain motionless with respect to the ground, even when the engines are firing their full power. This is obtained by declaring the corresponding XML elements empty:

```xml
<aerodynamics/>
<ground_reactions/>
```

### First stage

Basically there are 3 types of elements in a rocket stages:

- Engines (Merlin 1-D for the Falcon 9)
- Fuel
- Structure, which is basically everything that is neither fuel nor an engine (i.e. tank walls, interstage elements, actuation systems, pipes, electrical wires, etc.)

#### Merlin 1-D Engine

The first stage uses the Merlin 1-D engine that burns liquid oxygen (LOX) and RP-1 (highly refined kerosene).

| Dimension | Value |
|-|-|
| Sea Level thrust | 845 kN [(3)][3] |
| Vacuum thrust | 981 kN [(3)][3] |
| Dry weight | 470 kg [(3)][3] |
| Sea Level specific impulse | 282 s [(3)][3] |
| Vacuum specific impulse |  311 s [(3)][3] |
| Thrust to weight ratio | 185 [(3)][3] |

Given that the engine weight is `470 kg * 9.81 m/s^2 = 4611 N`, this gives a thrust to weight ratio of `845 / 4.611 = 183.26` which is consistent with the official thrust to weight ratio of 185.

In order to assess how much fuel is burnt by the engine, we use the specific impulse which is by definition the ratio of the thrust to the fuel flow weight: `ISP = F / (g0 * mdot)` where `F` is the engine thrust, `g0 = 9.80665 m/s^2` is the gravity at sea level and `mdot` is the mass fuel flow. Hence the fuel flow expression `mdot = F / (g0 * ISP)`

At sea level, we get a fuel flow of `845 kN / (9.80665 m/s^2 * 282 s) = 305.55 kg/s` and in vacuum `981 kN / (9.80665 m/s^2 * 311 s) = 321.65 kg/s`. For now, we will ignore that the Isp varies along the flight and **will assume that the Isp is constant and equal to the Merlin 1-D vacuum value**.

The LOX/RP-1 mixture burns with a mass ratio of 2.56 [(4)][4] so the mass fuel flow distribution is:

- `1 / (1 + 2.56) = 28.1%` of RP-1 i.e. `90.35 kg/s` in vacuum
- `2.56 / (1 + 2.56) = 71.9%` of LOX i.e. `231.30 kg/s` in vacuum

Finally according to [(3)][3] the minimum throttle is 40% (most likely below this threshold the engine flames out).

We now have the complete XML definition of our rocket engine for JSBSim (file `engine/Merlin_1-D.xml`):

```xml
<rocket_engine name="Merlin 1-D">
  <isp> 311 </isp>
  <maxthrottle> 1.0 </maxthrottle>
  <minthrottle> 0.4 </minthrottle>
  <slfuelflowmax unit="KG/SEC"> 90.35 </slfuelflowmax>
  <sloxiflowmax unit="KG/SEC"> 231.30 </sloxiflowmax>
</rocket_engine>
```

Given that the difference between vacuum thrust and sea level thrust is `981 - 845 = 136 kN` and that the sea level pressure is 101325 Pa then the throttle exit area is `136 kN / 101.325 kPa = 1.342 m^2`.

This gives the XML definition of the Merlin 1-D nozzle for JSBSim (file `engine/Merlin_1-D_nozzle.xml`):

```xml
<nozzle name="Merlin 1-D Nozzle">
  <area unit="M2"> 1.342 </area>
</nozzle>
```

#### Mass distribution

We need some approximation of the mass distribution so that JSBSim can assess reasonable inertias.

| Dimension | Value |
|-|-|
| Diameter | 3.66 m [(1)][1] |
| Height | 41.5 m [(2)][2] |
| Dry mass | 22.2 t [(1)][1] |
| Fuel mass | 475 t [(2)][2] |

The first stage is equipped with 9 Merlin 1-D engines so the structural mass is `22200 kg - 9*470 kg = 17970 kg` meaning that 20% of the overall structural mass is located at the very bottom of the first stage.

The structural mass will be assumed to be equally distributed along the stage which will be assumed to be a tube:

```xml
<pointmass name="First Stage Structure">
  <form shape="tube">
    <radius unit="FT"> 12 </radius>
    <length unit="M"> 41.5 </length>
  </form>
  <weight unit="KG"> 17970 </weight>
  <location unit="M">
    <x> 20.75 </x>
    <y> 0 </y>
    <z> 0 </z>
  </location>
</pointmass>
```

Each engine will be modeled by a `<pointmass>` element, once again so that JSBSim can properly assess the first stage inertia. One of the engine is located on the `X` axis line:

```xml
<pointmass name="Engine 1">
  <weight unit="KG"> 470 </weight>
  <location unit="M">
    <x> 0.0 </x>
    <y> 0.0 </y>
    <z> 0.0 </z>
  </location>
</pointmass>
```

The 8 remaining engines are distributed on a circle enclosed by the stage diameter. We will therefore assume that their nozzles are tangent to the stage external diameter. We determined previously that the throttle exit area is 1.342 m^2 meaning that the nozzle radius is 0.654m so the engines axes are placed on a radius of `3.66/2 - 0.654 = 1.176m`.

The typical definition of these engines will be:

```xml
<pointmass name="Engine 2">
  <weight unit="KG"> 470 </weight>
  <location unit="M">
    <x> 0.0 </x>
    <y> 1.176 </y>
    <z> 0.0 </z>
  </location>
</pointmass>
```

For the 7 other engines, the position `<y>`, `<z>` will be accomodated so that the 8 engines are placed symmetrically.

#### Fuel tanks

As described above, the engine burns a mixture ratio of 2.56 between LOX and RP-1 and the total fuel mass is 475t which allows to define the fuel mass for each of the RP-1 and LOX tanks:

- `475 t / (1 + 2.56) = 133.43t` of RP-1 with a density of 0.85 kg/l [(4)][4], [(6)][6] has a volume of 156.98 m^3. Assuming it is stored in a cylinder with the same diameter than the first stage (3.66 m), the tank height is 14.92 m.
- `475 t * 2.56 / (1 + 2.56) = 341.57t` of LOX with a density of 1.141 kg/l [(5)][5], [(6)][6] has a volume of 299.36 m^3. Assuming it is stored in a cylinder with the same diameter than the first stage (3.66 m), the tank height is 28.45 m.

It should be noted that the LOX tank is twice as big in volume than the RP-1 tank.

As a result, the total height of the two tanks would be `14.92 + 28.45 = 43.37 m` which is more than the stage overall height (41.5 m): _the numbers don't add up_ but we will ignore that for now.

We will assume that the RP-1 tank is below the LOX tank so the centers of gravity (CG) of each tank are:

- RP-1 tank: `14.92 / 2 = 7.46 m`
- LOX tank: `28.45/2 + 7.46 = 21.69m`

The XML definition of the RP-1 tank is then:

```xml
<tank type="FUEL">
  <location unit="M">
    <x> 7.46 </x>
    <y> 0.0 </y>
    <z> 0.0 </z>
  </location>
  <capacity unit="KG"> 133430 </capacity>
  <contents unit="KG"> 133430 </contents>
</tank>
```

and the XML definition of the LOX tank is:

```xml
<tank type="OXIDIZER">
  <location unit="M">
    <x> 21.69 </x>
    <y> 0.0 </y>
    <z> 0.0 </z>
  </location>
  <capacity unit="KG"> 341570 </capacity>
  <contents unit="KG"> 341570 </contents>
</tank>
```

#### Putting everything together

Now we need to specify each of the engines used by the rocket. This is mostly some glue to put everything together: for each engine we have to specify where its definition is located (i.e. the file `Merlin_1-D.xml` for the engine itself, and `Merlin_1-D_nozzle.xml` for its nozzle) and which tanks feed the engine (here tanks 0 "FUEL" and tank 1 "OXIDIZER"):

```xml
<engine file="Merlin_1-D">
    <feed> 0 </feed>
    <feed> 1 </feed>
    <thruster file="Merlin_1-D_nozzle">
        <location unit="M">
            <x> 0.0 </x>
            <y> 0.0 </y>
            <z> 0.0 </z>
        </location>
    </thruster>
</engine>
```

### 2nd Stage and upper elements

In order to keep the model simple for a start, we will replace the other parts of the rocket by a single `<pointmass>` that holds the rest of the rocket mass.

The rocket dimensions are the following

| Dimension | Value |
|-|-|
| Take Off mass | 549 t [(1)][1]|
| Height | 70 m [(1)][1] |

The first stage is equal to the sum of the dry mass (22.2t) and of the fuel (475t), so the upper elements mass is `549 - 475 - 22.2 = 51.8t` and height is `70 - 41.5 = 28.5 m`. For now, we will assume at the 74t are entirely located at the mid height of the upper elements i.e. at `41.5 + 28.5/2 = 55.75m`:

```xml
<pointmass name="Upper Elements">
    <weight unit="KG"> 51800 </weight>
    <location unit="M">
        <x> 55.75 </x>
        <y> 0.0 </y>
        <z> 0.0 </z>
    </location>
</pointmass>
```

This is not meant to be a realistic replacement of the upper stages but rather a means to keep the model easier to digest.

## Static Firing Test

This first test is meant to check the vehicle mass and thrust at sea level. The rocket will be held down during the test so that it remains motionless and we will use JSBSim to gather the information we seek (i.e. CG/mass/inertia and sea level thrust).

### Initial conditions

The static firing test will be conducted from the [Kennedy Space Center launch complex 39 (LC39) which is located at latitude and longitude 28.6269702 deg ,-80.6237944 deg](https://goo.gl/maps/m3UVULKKZZwhqwB1A). Even though the launch pad is slightly higher than the sea level, for simplicity we will assume that its altitude is 0 ft.

By default, JSBSim initializes a vehicle with the `X` axis horizontal so we need to rotate the rocket to get the proper orientation (i.e. the rocket nose up). This is obtained by setting the `theta` (i.e. pitch) angle to 90 degrees.

```xml
<initialize name="LC39">
    <latitde unit="DEG">    28.6269702 </latitde>
    <longitude unit="DEG"> -80.6237944 </longitude>
    <theta unit="DEG"> 90.0 </theta>
    <altitude unit="FT"> 0.0 </altitude>
</initialize>
```

### Script

JSBSim can run scripts to execute a sequence of events and display notifications. This is an easy means to create a reproducible test of a model. For the purpose of running a static firing of our Falcon 9 model, we will include 5 events in the script:

1. Start engine #0 (the one located on the axis of symmetry).
2. Check the thrust produced by engine #0. The purpose of this event is to display values from JSBSim in the console.
3. Check the fuel consumption. This event displays the tanks content 1 second after the previous event so that we can check that the contents has been reduced by the expected amount.
4. Starts the other 8 engines.
5. Display the thrust of all 9 engines as well as the total thrust so that we can validate that we are producing the expected amount of thrust.

The script is to stored in the file `scripts/Falcon9_static_firing.xml`

First, we must indicate JSBSim that we are using the Falcon 9 model with the initial conditions at the launch complex LC39:

```xml
<use aircraft="Falcon9" initialize="LC39"/>
```

The script will simulate a run of 5 seconds with a time step of 0.005s (200 Hz). We will also specify JSBSim to hold down the rocket (i.e. to keep it motionless) by setting the property `forces/hold-down` to 1.

```xml
<run start="0.0" end="5.0" dt="0.005">
  <property value="1"> forces/hold-down </property>
</run>
```

Our first event consists in starting engine #0 (by setting the property `fcs/throttle-cmd-norm[0]` to 1.0). The event will occur when the elapsed simulation time `simulation/sim-time-sec` has reached 0.1 second which is specified in the `<condition>` element:

```xml
<event name="Start engine #0">
  <condition> simulation/sim-time-sec ge 0.1 </condition> <!-- "ge" means "greater than or equal to" -->
  <set name="fcs/throttle-cmd-norm[0]" value="1.0"/>
  <notify/> <!-- display a message in the console when the event is triggered -->
</event>
```

The choice of triggering the event after 0.1 second is arbitrary; any elapsed time is acceptable for this event as long as it occurs first.

The second event will display the thrust of engine #0 (property `propulsion/engine[0]/thrust-lbs`), the rocket total thrust (property `forces/fbx-prop-lbs` which the resultant force along the `X` axis for propulsion), the fuel and oxidizer tank contents (resp. properties `propulsion/tank[0]/contents-lbs` and `propulsion/tank[1]/contents-lbs`):

```xml
<event name="Engine #0 Thrust check">
    <condition> simulation/sim-time-sec ge 1.0 </condition>
    <notify>
        <property caption="Engine #0 thrust (lbf)     "> propulsion/engine[0]/thrust-lbs </property>
        <property caption="Fuel tank content (lbs)    "> propulsion/tank[0]/contents-lbs </property>
        <property caption="Oxidizer tank content (lbs)"> propulsion/tank[1]/contents-lbs </property>
        <property caption="Total thrust (lbf)         "> forces/fbx-prop-lbs </property>
    </notify>
</event>
```

This event is triggered after the elapsed simulation time is greater than 1.0 second to make sure that the engine has built up its full thrust. The `caption` attribute used by the `<property>` elements allows to get meaningful text messages in the console instead of the cryptic property names.

The next event purpose is to check that the fuel and oxidizer mass flows are correct. The idea is therefore to wait for 1.0 second and display the tank contents again.

```xml
<event name="Engine #0 Fuel consumption check">
    <condition> simulation/sim-time-sec ge 2.0 </condition>
    <notify>
        <property caption="Engine #0 thrust (lbf)     "> propulsion/engine[0]/thrust-lbs </property>
        <property caption="Fuel tank content (lbs)    "> propulsion/tank[0]/contents-lbs </property>
        <property caption="Oxidizer tank content (lbs)"> propulsion/tank[1]/contents-lbs </property>
        <property caption="Total thrust (lbf)         "> forces/fbx-prop-lbs </property>
    </notify>
</event>
```

Now we want to check that once the 9 engines are started, the total thrust meet the specification (i.e. approx. 7600kN). We will proceed with 2 events, the first one will start the engines by setting their throttle command to `1.0` and the second event will display the thrust after having waited for 1.0 second after ignition to make sure that all engines have reached their maximum thrust.

```xml
<event name="Start all engines">
    <condition> simulation/sim-time-sec ge 3.0 </condition>
    <set name="fcs/throttle-cmd-norm[1]" value="1.0"/>
    <set name="fcs/throttle-cmd-norm[2]" value="1.0"/>
    <set name="fcs/throttle-cmd-norm[3]" value="1.0"/>
    <set name="fcs/throttle-cmd-norm[4]" value="1.0"/>
    <set name="fcs/throttle-cmd-norm[5]" value="1.0"/>
    <set name="fcs/throttle-cmd-norm[6]" value="1.0"/>
    <set name="fcs/throttle-cmd-norm[7]" value="1.0"/>
    <set name="fcs/throttle-cmd-norm[8]" value="1.0"/>
    <notify/>
</event>

<event name="Total thrust check">
    <condition> simulation/sim-time-sec ge 4.0 </condition>
    <notify>
        <property caption="Engine #0 thrust (lbf)"> propulsion/engine[0]/thrust-lbs </property>
        <property caption="Engine #1 thrust (lbf)"> propulsion/engine[1]/thrust-lbs </property>
        <property caption="Engine #2 thrust (lbf)"> propulsion/engine[2]/thrust-lbs </property>
        <property caption="Engine #3 thrust (lbf)"> propulsion/engine[3]/thrust-lbs </property>
        <property caption="Engine #4 thrust (lbf)"> propulsion/engine[4]/thrust-lbs </property>
        <property caption="Engine #5 thrust (lbf)"> propulsion/engine[5]/thrust-lbs </property>
        <property caption="Engine #6 thrust (lbf)"> propulsion/engine[6]/thrust-lbs </property>
        <property caption="Engine #7 thrust (lbf)"> propulsion/engine[7]/thrust-lbs </property>
        <property caption="Engine #8 thrust (lbf)"> propulsion/engine[8]/thrust-lbs </property>
        <property caption="Total thrust (lbf)    "> forces/fbx-prop-lbs </property>
    </notify>
</event>
```

### Simulation Run

The simulation is run by instructing JSBSim to execute the script. This is done with the command line:

```bash
> JSBSim scripts/Falcon_static_firing.xml
```

By default, JSBSim executes the simulation as fast as the CPU can and the execution takes much less time than the 5 seconds of simulated time.

A number of messages will be displayed in the console. JSBSim initially logs a lot of information about the model and how it has interpreted the data that it has provided with. We will ignore that as long as no error message has been issued.

#### Mass Properties Report

First, we will focus on the mass properties report to check that we have correctly specified the masses and CGs (Centers of Gravity):

```bash
  Mass Properties Report (English units: lbf, in, slug-ft^2)
                                      Weight    CG-X    CG-Y    CG-Z         Ixx         Iyy         Izz         Ixy         Ixz         Iyz
    Base Vehicle                         0.0     0.0     0.0     0.0         0.0         0.0         0.0        -0.0         0.0        -0.0
0   First Stage Structure            39617.1   816.9     0.0     0.0    177312.4   1990880.1   1990880.1         0.0         0.0         0.0
1   Engine 0                          1036.2     0.0     0.0     0.0         0.0         0.0         0.0        -0.0         0.0        -0.0
2   Engine 1                          1036.2     0.0    46.3     0.0         0.0         0.0         0.0        -0.0         0.0        -0.0
3   Engine 2                          1036.2     0.0    32.8    32.8         0.0         0.0         0.0        -0.0         0.0        -0.0
4   Engine 3                          1036.2     0.0     0.0    46.3         0.0         0.0         0.0        -0.0         0.0        -0.0
5   Engine 4                          1036.2     0.0   -32.8    32.8         0.0         0.0         0.0        -0.0         0.0        -0.0
6   Engine 5                          1036.2     0.0   -46.3     0.0         0.0         0.0         0.0        -0.0         0.0        -0.0
7   Engine 6                          1036.2     0.0   -32.8   -32.8         0.0         0.0         0.0        -0.0         0.0        -0.0
8   Engine 7                          1036.2     0.0     0.0   -46.3         0.0         0.0         0.0        -0.0         0.0        -0.0
9   Engine 8                          1036.2     0.0    32.8   -32.8         0.0         0.0         0.0        -0.0         0.0        -0.0
10  Upper Elements                  114199.5  2194.9     0.0     0.0         0.0         0.0         0.0        -0.0         0.0        -0.0
0   Fuel                              294163 293.701       0       0           0           0           0
1   Oxidizer                          753033 853.937       0       0           0           0           0

    Total:                         1210337.8   836.5     0.0     0.0    181149.8  67642667.8  67642667.8        -0.0         0.0         0.0
```

This report shows each element that we have specified in the model with its weight (actually its mass), CG position (X, Y, Z) and the inertia tensor (Ixx, Iyy, Izz, Ixy, Ixz, Iyz).

JSBSim uses British/Imperial units so the mass are reported in pounds (lbs), the CG position in inches (in) and the inertias in slugs square feet (slugs*ft^2). Below is the conversion to SI units and comparison to specs:

| Element | Weight (lbs) | Mass (kg) | Falcon 9 specs (kg) |
|-|-|-|-|
| First Stage Structure | 39617.1 | 17970.0 | 17 970 |
| Engines (0 to 8) | 1036.2 | 470.0 | 470 |
| Upper Elements | 114199.5 | 51800.0 | 51 800 |
| Fuel (RP-1) | 294163 | 133430.1 | 133 430 |
| Oxidizer (LOX) | 753033 | 341570.0 | 341 570 |
| **Total** | **1210337.8** | **549000.0** | **549 000** |

The weights reported by JSBSim match the specified values. Also the total value meets the specification so we have not overlooked any major contributor to the mass, CG and inertia of the rocket.

| Element | CG x (in) | CG x (m) | Specs (m) |
|-|-|-|-|
| First Stage Structure | 816.9 | 20.75 | 20.75 |
| Upper Elements | 2194.9 | 55.75 | 55.75 |
| Fuel (RP-1) | 293.701 | 7.46 | 7.46 |
| Oxidizer (LOX) | 853.937 | 21.69 | 21.69 |

There again the CG `X` coordinate match the specified values.

#### Position, Orientation and Velocities

JSBSim also issues a state report about the position, orientation, translational and rotational velocities:

```text
State Report at sim time: 0.000000 seconds
  Position
    ECI:   2990075.469750493, -18108249.26885875, 10017837.84346803 (x,y,z, in ft)
    ECEF:  2990075.469750 , -18108249.268859 , 10017837.843468 (x,y,z, in ft)
    Local: 28.789108, -80.623794, 0.000000 (geodetic lat, lon, alt ASL in deg and ft)

  Orientation
    ECI:   -1.814560150233314e-15, -28.78910751088395, 279.3762056 (phi, theta, psi in deg)
    Local: 1.67391703968785e-15, 90, 0 (phi, theta, psi in deg)

  Velocity
    ECI:   1320.474361171839, 218.0397418409962, 0 (x,y,z in ft/s)
    ECEF:  0, 0, 0 (x,y,z in ft/s)
    Local: 0.000000 , 0.000000 , 0.000000 (n,e,d in ft/sec)
    Body:  0.000000 , 0.000000 , 0.000000 (u,v,w in ft/sec)

  Body Rates (relative to given frame, expressed in body frame)
    ECI:   0.002012106483392044, -1.159648525051017e-19, 0.003661656859126495 (p,q,r in deg/s)
    ECEF:  0, 0, 0 (p,q,r in deg/s)
```

The position is given in 3 different coordinate systems:

- ECI - Earth Centered Inertial. As the name implies, this frame origin is located at the center of the Earth but it does not follow the Earth rotation (i.e. its axes always point to the same stars).
- ECEF - Earth Centered Earth rotating Frame. Same as ECI except that this frame follows the Earth rotation (i.e. its axes are fixed with respect to the Earth).
- Local - This one uses coordinates that we are more familiar with: latitude, longitude and altitude.

The coordinates in the Local frame matches our initial conditions specifications.

The local orientation describes the orientation with respect to the local horizon and we see that `theta` is equal to 90 degrees while the other angles (`phi` and `psi`) are zero, as specified.

Translational velocities (`Velocity`) are zero for all frames except the ECI. This is no surprise since our rocket is motionless with respect to the Earth surface so it follows the Earth rotation. The velocities with respect to the inertial frame are non zero. This is of importance when launching to orbit since this gives the rocket some initial velocity "for free".

Rotational velocities (`Body Rates`) are zero in the ECEF confirming there again that the rocket is motionless with respect to Earth. However the rotational velocities are not zero in the ECI because the rocket rotates with the Earth (360 degrees every 24h i.e. 4.17e-3 deg/s) which are distributed over the roll rate `p` and the yaw rate `r` due to the fact that the rocket `X` axis is not parallel to the Earth axis of rotation.

#### Engine 0 Thrust

JSBSim reports the first and second events occurrence:

```text
Start engine #0 (Event 0) executed at time: 0.100000


Engine #0 Thrust check (Event 1) executed at time: 1.000000
    Engine #0 thrust (lbf)      = 189966.070092
    Fuel tank content (lbs)     = 293983.527545
    Oxidizer tank content (lbs) = 752574.012654
    Total thrust (lbf)          = 189966.070092
```

This confirms that the first event (Start engine 0) has been executed after 0.1s of simulated elapsed time. The thrust of engine 0 is 189966 lbf i.e. 844.97 kN which is close enough to the specified value of 845 kN.

Of course, the total thrust is equal to the thrust of one engine since all the other engines are off.

#### Fuel consumption

After another second of simulated elapsed time (actually 1.005 second since the elapsed time is 2.005), JSBSim displays the fuel and oxidizer contents as well as the thrust:

```text
Engine #0 Fuel consumption check (Event 2) executed at time: 2.005000
    Engine #0 thrust (lbf)      = 189966.062494
    Fuel tank content (lbs)     = 293783.343953
    Oxidizer tank content (lbs) = 752061.533795
    Total thrust (lbf)          = 189966.062494
```

The engine thrust and the rocket total thrust are almost unchanged so we can safely assume that the fuel and oxidizer have been constant during the elapsed second.

The fuel content has changed from 293983.527 lbs to 293783.343 lbs in 1.005 second so the burnt mass is `293983.527 - 293783.343 = 200.184 lbs` i.e. a mass flow rate of `200.184 / 1.005 = 199.188 lb/s` (90.35 kg/s in SI unit). This is compliant with the specified value.

Similarly, the oxidizer content has changed `752574.013 - 752061.534 = 512.479 lbs` and the mass flow rate is 231.3 kg/s as per the specified value.

#### Rocket total thrust

Finally, all the engines are started and individual and cumulated thrust are displayed:

```text
Start all engines (Event 3) executed at time: 3.005000


Total thrust check (Event 4) executed at time: 4.005000
    Engine #0 thrust (lbf) = 189965.987016
    Engine #1 thrust (lbf) = 189965.987016
    Engine #2 thrust (lbf) = 189965.987016
    Engine #3 thrust (lbf) = 189965.987016
    Engine #4 thrust (lbf) = 189965.987016
    Engine #5 thrust (lbf) = 189965.987016
    Engine #6 thrust (lbf) = 189965.987016
    Engine #7 thrust (lbf) = 189965.987016
    Engine #8 thrust (lbf) = 189965.987016
    Total thrust (lbf)     = 1709693.883144
```

All 9 engines have the same identical thrust of 189966 lbf (i.e. 844.97kN close enough to the specified 845 kN) and the cumulated thrust is 1709694 lbf i.e. 7604.7kN which is close enough to the Falcon 9 official specification of 7607 kN [(7)][7].

## Wrap up

We have built a simple model of the Falcon 9 rocket. For now, we have detailed the first stage of the rocket and simplified the rest of the rocket (2nd stage, fairing, payload, etc.) with a point mass.

We have checked that the mass and CG of the model as per our specifications and that the individual and cumulated thrust of the engines match the specification.

Now is the time to launch our rocket to the stars !

## Sources

- (1) [https://en.wikipedia.org/wiki/Falcon_9][1]
- (2) [https://fr.wikipedia.org/wiki/Falcon_9][2]
- (3) [https://en.wikipedia.org/wiki/SpaceX_Merlin][3]
- (4) [https://en.wikipedia.org/wiki/RP-1][4]
- (5) [https://en.wikipedia.org/wiki/Liquid_oxygen][5]
- (6) [http://www.braeunig.us/space/propel.htm][6]
- (7) [https://www.spacex.com/vehicles/falcon-9/][7]

[1]: https://en.wikipedia.org/wiki/Falcon_9
[2]: https://fr.wikipedia.org/wiki/Falcon_9
[3]: https://en.wikipedia.org/wiki/SpaceX_Merlin
[4]: https://en.wikipedia.org/wiki/RP-1
[5]: https://en.wikipedia.org/wiki/Liquid_oxygen
[6]: http://www.braeunig.us/space/propel.htm
[7]: https://www.spacex.com/vehicles/falcon-9/
