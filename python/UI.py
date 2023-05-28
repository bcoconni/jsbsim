# A Graphical User Interface to JSBSim
#
# Copyright (c) 2023 Bertrand Coconnier
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License along
# with this program; if not, write to the Free Software Foundation, Inc., 59
# Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# Further information about the GNU Lesser General Public License can also be
# found on the world wide web at http://www.gnu.org.

import tkinter as tk
from tkinter import ttk


class PropertyList(ttk.Treeview):
    def __init__(self, master, properties, get_property_value):
        super().__init__(master, columns=("prop", "val"))

        self.heading("prop", text="Name", anchor=tk.W)
        self.heading("val", text="Values", anchor=tk.W)
        self.column("#0", width=60)

        leafs = {}

        for p in sorted(properties):
            node = ""
            for name in p.split("/"):
                parent = node
                node = "/".join((parent, name))
                if node in leafs:
                    continue

                display_name = "  " * parent.count("/") + name
                if parent:
                    piid = leafs[parent]
                else:
                    piid = ""
                leafs[node] = self.insert(
                    piid, tk.END, values=(display_name, ""), open=False
                )

            self.set(leafs["/" + p], "val", get_property_value(p))

    def get_selected_properties(self):
        selected_prop = []
        for selected_item in self.selection():
            item = self.item(selected_item)
            record = item["values"]
            name = record[0].strip()

            parent = self.parent(selected_item)
            while parent:
                item = self.item(parent)
                name = item["values"][0].strip() + "/" + name
                parent = self.parent(parent)

            if not self.get_children([selected_item]):
                selected_prop.append((name, record[1]))

        return selected_prop


class WatchList(ttk.Treeview):
    def __init__(self, master):
        super().__init__(master, columns=("prop", "val"), show="headings")

        self.heading("prop", text="Name", anchor=tk.W)
        self.heading("val", text="Values", anchor=tk.W)

    def add_property(self, name, value):
        # Avoid duplicating properties that are already in the watch list.
        for child in self.get_children():
            item = self.item(child)
            child_name = item["values"][0]
            if child_name == name:
                return

        self.insert("", tk.END, values=(name, value))

    def update(self, get_property_value):
        for child in self.get_children():
            item = self.item(child)
            record = item["values"]
            self.set(child, "val", get_property_value(record[0]))

    def remove_selected_properties(self):
        for selected_item in self.selection():
            self.delete((selected_item,))


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

        self.property_list = PropertyList(frame, properties, fdm.get_property_value)
        self.property_list.grid(column=0, row=1)

        self.watch_list = WatchList(frame)
        self.watch_list.grid(column=1, row=1)

        btn = ttk.Button(frame, text="Watch")
        btn.bind("<Button>", self.watch_properties)
        btn.grid(column=0, row=2)

        btn = ttk.Button(frame, text="Unwatch")
        btn.bind("<Button>", lambda event: self.watch_list.remove_selected_properties())
        btn.grid(column=1, row=2)

        btn = ttk.Button(frame, text="Step")
        btn.bind("<Button>", self.step)
        btn.grid(column=0, row=3)

        ttk.Button(self, text="Quit", command=lambda: self.quit()).pack()

    def watch_properties(self, event):
        for item in self.property_list.get_selected_properties():
            self.watch_list.add_property(item[0], item[1])

    def step(self, event):
        self.fdm.run()
        self.watch_list.update(self.fdm.get_property_value)
