import tkinter as tk
from tkinter import ttk


class App(tk.Tk):
    def __init__(self, fdm):
        super().__init__()
        aircraft_name = fdm.get_aircraft().get_aircraft_name()
        self.title(f"JSBSim [{aircraft_name}]")
        self.fdm = fdm

        frame = ttk.Frame(self)
        frame.pack()

        label = ttk.Label(frame, text="Properties")
        label.grid(column=0, row=0)
        label = ttk.Label(frame, text="Watch List")
        label.grid(column=1, row=0)

        properties = [
            p.split(" ")[0]
            for p in fdm.query_property_catalog("").split("\n")
            if len(p) != 0
        ]

        self.property_list = ttk.Treeview(frame, columns=("prop", "val"))
        self.property_list.heading("prop", text="Name", anchor=tk.W)
        self.property_list.heading("val", text="Values", anchor=tk.W)

        leafs = {}
        count = 0

        for p in sorted(properties):
            node = ""
            for name in p.split("/"):
                parent = node
                node = "/".join((parent, name))
                if node in leafs:
                    continue

                display_name = "  " * parent.count("/") + name
                iid = self.property_list.insert(
                    "", tk.END, values=(display_name, ""), open=False
                )
                leafs[node] = iid
                count += 1
                if parent:
                    piid = leafs[parent]
                    children = self.property_list.get_children([piid])
                    self.property_list.move(iid, piid, len(children))

            self.property_list.set(leafs["/" + p], "val", fdm.get_property_value(p))

        self.property_list.grid(column=0, row=1)

        self.watch_list = ttk.Treeview(frame, columns=("prop", "val"), show="headings")
        self.watch_list.heading("prop", text="Name", anchor=tk.W)
        self.watch_list.heading("val", text="Values", anchor=tk.W)
        self.watch_list.grid(column=1, row=1)

        btn = ttk.Button(frame, text="Watch")
        btn.bind("<Button>", self.watch_properties)
        btn.grid(column=0, row=2)

        btn = ttk.Button(frame, text="Unwatch")
        btn.bind("<Button>", self.unwatch_properties)
        btn.grid(column=1, row=2)

        btn = ttk.Button(frame, text="Step")
        btn.bind("<Button>", self.step)
        btn.grid(column=0, row=3)

        ttk.Button(self, text="Quit", command=lambda: self.quit()).pack()

    def watch_properties(self, event):
        for selected_item in self.property_list.selection():
            item = self.property_list.item(selected_item)
            record = item["values"]
            name = record[0].strip()

            parent = self.property_list.parent(selected_item)
            while parent:
                item = self.property_list.item(parent)
                name = item["values"][0].strip() + "/" + name
                parent = self.property_list.parent(parent)

            if not self.property_list.get_children([selected_item]):
                # Avoid duplicating properties that are laredy in the watch list.
                for child in self.watch_list.get_children():
                    item = self.watch_list.item(child)
                    child_name = item["values"][0]
                    if child_name == name:
                        break
                else:
                    self.watch_list.insert("", tk.END, values=(name, record[1]))

    def unwatch_properties(self, event):
        for selected_item in self.watch_list.selection():
            self.watch_list.delete((selected_item,))

    def step(self, event):
        self.fdm.run()
        for child in self.watch_list.get_children():
            item = self.watch_list.item(child)
            record = item["values"]
            self.watch_list.set(child, "val", self.fdm.get_property_value(record[0]))


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
