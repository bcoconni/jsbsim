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
from tkinter import filedialog as fd
from tkinter.messagebox import showinfo

import os
import matplotlib
import numpy as np

matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from matplotlib.figure import Figure


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


class Graph:
    def __init__(self, master):
        figure = Figure(figsize=(8, 4), dpi=100)
        self.axes = figure.add_subplot()
        self.lines = {}
        self.axes.set_xlabel("time [s]")
        self.axes.grid()

        self.canvas = FigureCanvasTkAgg(figure, master)
        NavigationToolbar2Tk(self.canvas, master, pack_toolbar=True)

        self.canvas.get_tk_widget().pack()

    def update(self, time, history):
        for name in history.keys():
            data = history[name]

            if name in self.lines:
                line = self.lines[name]
                line.set_data(time, data)
                line.set_label(name)
            else:
                (self.lines[name],) = self.axes.plot(time, data, label=name)
        self.axes.legend()
        self.axes.relim()
        self.axes.autoscale_view()
        self.canvas.draw()


class MenuBar(tk.Menu):
    def __init__(self, root, root_dir):
        super().__init__(root)
        self.root_dir = root_dir

        file_menu = tk.Menu(self, tearoff=False)
        file_menu.add_command(label="Open...", command=self.select_script_file)
        file_menu.add_command(label="Exit", command=root.destroy)
        self.add_cascade(label="File", menu=file_menu)

    def select_script_file(self):
        filename = fd.askopenfilename(
            title="Open a script",
            initialdir=os.path.join(self.root_dir, "scripts"),
            filetypes=(("script files", "*.xml"),),
        )
        if filename:
            showinfo(
                title="Selected script",
                message=os.path.relpath(filename, self.root_dir),
            )


class App(tk.Tk):
    def __init__(self, fdm, root_dir="."):
        super().__init__()
        self.root_dir = root_dir
        aircraft_name = fdm.get_aircraft().get_aircraft_name()
        self.title(f"JSBSim [{aircraft_name}]")
        self.fdm = fdm

        menubar = MenuBar(self, root_dir)
        self.config(menu=menubar)

        frame = ttk.Frame(self)
        frame.pack()

        self.graph = Graph(frame)

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
        btn.bind("<Button>", self.watch_selected_properties)
        btn.grid(column=0, row=2)

        btn = ttk.Button(frame, text="Unwatch")
        btn.bind("<Button>", lambda event: self.watch_list.remove_selected_properties())
        btn.grid(column=1, row=2)

        frame = ttk.Frame(self)

        btn = ttk.Button(frame, text="Step")
        btn.bind("<Button>", self.step)
        btn.grid(column=0, row=0)

        btn = ttk.Button(frame, text="Run")
        btn.bind("<Button>", self.run)
        btn.grid(column=1, row=0)

        btn = ttk.Button(frame, text="Pause")
        btn.bind("<Button>", self.pause)
        btn.grid(column=2, row=0)

        frame.pack()

        self.time = []
        self.prop_history = {}

    def watch_selected_properties(self, _):
        for item in self.property_list.get_selected_properties():
            self.watch_list.add_property(item[0], item[1])
            self.prop_history[item[0]] = np.zeros((len(self.time),))

    def step(self, _):
        self.fdm.run()
        self.watch_list.update(self.fdm.get_property_value)
        self.update_graph()

    def update_graph(self):
        self.time.append(self.fdm.get_sim_time())

        if self.prop_history:
            for prop in self.prop_history:
                self.prop_history[prop] = np.append(
                    self.prop_history[prop], self.fdm.get_property_value(prop)
                )
            self.graph.update(self.time, self.prop_history)

    def run(self, _):
        self.update_id = self.watch_list.after(250, self.update)

    def pause(self, _):
        self.watch_list.after_cancel(self.update_id)

    def update(self):
        for _ in range(50):
            self.fdm.run()
        self.watch_list.update(self.fdm.get_property_value)
        self.update_graph()
        self.update_id = self.watch_list.after(250, self.update)
