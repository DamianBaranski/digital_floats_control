import tkinter as tk
from tkinter import ttk, messagebox
from .monitoring_data import MonitoringData
class MonitoringTable(tk.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        self.monitoring_list = []
        self.create_treeview()
        self.dragged_item = None  # Track the item being dragged

    def create_treeview(self):
        # Create a frame for the Treeview and Scrollbars
        frame = tk.Frame(self)
        frame.pack(fill='both', expand=True)

        # Create the Treeview
        self.tree = ttk.Treeview(frame, selectmode='browse')
        self.tree.pack(side='left', fill='both', expand=True)

        # Create a horizontal scrollbar
        self.h_scrollbar = ttk.Scrollbar(self, orient='horizontal', command=self.tree.xview)
        self.h_scrollbar.pack(side='bottom', fill='x')

        # Configure the Treeview to use the scrollbars
        self.tree.configure(xscrollcommand=self.h_scrollbar.set)

        # Create columns
        column_names = [
            'Voltage',
            'Current',
            'State',
            'Up switch',
            'Down switch'
        ]
        self.tree['columns'] = column_names  # Include an index column
        self.tree['show'] = "headings"

        # Create headings and set column widths
        for col in column_names:
            self.tree.heading(col, text=col)
            self.tree.column(col, anchor="center", width=150)

        # Insert multiple rows
        self.addData(6)
        self.populate_treeview()

    def populate_treeview(self):
        # Clear existing rows
        for row in self.tree.get_children():
            self.tree.delete(row)

        # Insert rows for each settings instance
        for monitoringData in self.monitoring_list:
            row_values = [monitoringData[col] for col in range(len(self.tree['columns']))]
            self.tree.insert('', 'end', values=row_values)

    def update_row(self, monitoringData):
        # Update the Treeview with new values
        self.populate_treeview()

    def addData(self, idx):
        while len(self.monitoring_list) < idx:
            self.monitoring_list.append(['N/A', 'N/A', 'N/A', 'N/A', 'N/A'])
        
    def setData(self, row, monitoringData):
        """Set the ChannelSettings object for a specific row."""
        if 0 <= row < len(self.monitoring_list):
            row_values = [monitoringData.getVoltage(), monitoringData.getCurrent(), monitoringData.getState(), monitoringData.getUpSwitch(), monitoringData.getDownSwitch()]
            self.monitoring_list[row] = row_values
            self.populate_treeview()
        else:
            raise IndexError("Row index out of range.")

    def getData(self, row):
        """Get the ChannelSettings object for a specific row."""
        if 0 <= row < len(self.monitoring_list):
            return self.monitoring_list[row]
        else:
            raise IndexError("Row index out of range.")

    def delData(self, row):
        """Delete the ChannelSettings object at a specific row."""
        if 0 <= row < len(self.monitoring_list):
            del self.monitoring_list[row]
            self.populate_treeview()
        else:
            raise IndexError("Row index out of range.")