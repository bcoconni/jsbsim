#!/usr/bin/python
import math

from UI import App

if __name__ == "__main__":

    class dummyAircraft:
        def __init__(self, name):
            self.name = name

        def get_aircraft_name(self):
            return self.name

    class dummyFDM:
        def __init__(self):
            self.properties = {
                "a/c": -1.0,
                "a/b": 2.0,
                "c/d/e": 4.0,
                "b/c": 1.0,
                "c/d/a": 3.0,
            }
            self.time = 0.0

        def query_property_catalog(self, _):
            prop_names = list(self.properties.keys()) + [""]
            return " (RW)\n".join(prop_names)

        def get_property_value(self, name):
            return self.properties[name]

        def get_aircraft(self):
            return dummyAircraft("Test")

        def run(self):
            self.properties["a/b"] = 2.0 * math.cos(0.15 * math.pi * self.time)
            self.properties["c/d/e"] = 4.0 * math.cos(0.5 * math.pi * self.time)
            self.time += 0.01

        def get_sim_time(self):
            return self.time

    app = App(dummyFDM())
    app.mainloop()
