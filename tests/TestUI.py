#!/usr/bin/python
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
                "a/c": 2.0,
                "a/b": 0.0,
                "c/d/e": 4.0,
                "b/c": 1.0,
                "c/d/a": 3.0,
            }

        def query_property_catalog(self, _):
            prop_names = list(self.properties.keys()) + [""]
            return " (RW)\n".join(prop_names)

        def get_property_value(self, name):
            return self.properties[name]

        def get_aircraft(self):
            return dummyAircraft("Test")

        def run(self):
            self.properties["a/b"] += 0.25
            self.properties["c/d/e"] -= 0.1

    app = App(dummyFDM())
    app.mainloop()
