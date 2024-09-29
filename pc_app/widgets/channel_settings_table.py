import tkinter as tk
from tkinter import ttk, messagebox
from .channel_settings import ChannelSettings


class ChannelSettingsTable(tk.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        self.channel_settings_list = []
        self.create_treeview()
        self.dragged_item = None  # Track the item being dragged

    def create_treeview(self):
        # Create a frame for the Treeview and Scrollbars
        frame = tk.Frame(self)
        frame.pack(fill='both', expand=True)

        # Create the Treeview
        self.tree = ttk.Treeview(frame, selectmode='browse')
        self.tree.pack(side='left', fill='both', expand=True)

        # Create a vertical scrollbar
        self.v_scrollbar = ttk.Scrollbar(frame, orient='vertical', command=self.tree.yview)
        self.v_scrollbar.pack(side='right', fill='y')

        # Create a horizontal scrollbar
        self.h_scrollbar = ttk.Scrollbar(self, orient='horizontal', command=self.tree.xview)
        self.h_scrollbar.pack(side='bottom', fill='x')

        # Configure the Treeview to use the scrollbars
        self.tree.configure(yscrollcommand=self.v_scrollbar.set, xscrollcommand=self.h_scrollbar.set)

        # Create columns
        column_names = [
            'enable',
            'bridge',
            'inverse_motor',
            'inverse_up_limit_switch',
            'inverse_down_limit_switch',
            'inverse_limit_switch',
            'rudder',
            'ina_addr',
            'ina_calibration',
            'pcf_addr',
            'pcf_channel',
            'max_voltage_limit',
            'min_voltage_limit',
            'max_current_limit',
            'min_current_limit'
        ]
        self.tree['columns'] = column_names  # Include an index column
        self.tree['show'] = "headings"

        # Create headings and set column widths
        for col in column_names:
            self.tree.heading(col, text=col)
            self.tree.column(col, anchor="center", width=150)

        # Insert multiple rows
        self.populate_treeview()

        # Bind the double-click event to edit a row
        self.tree.bind('<Double-1>', self.edit_row)

        # Bind mouse events for drag and drop
        self.tree.bind('<ButtonPress-1>', self.on_item_press)
        self.tree.bind('<B1-Motion>', self.on_item_drag)
        self.tree.bind('<ButtonRelease-1>', self.on_item_release)

    def display_instructions(self):
        """Display instructions for editing and dragging items."""
        instruction_message = (
            "Instructions:\n"
            "- Double-click a row to edit the settings.\n"
            "- Click and drag a row to reorder.\n"
            "- Use the scrollbars to navigate."
        )
        messagebox.showinfo("Instructions", instruction_message)

    def populate_treeview(self):
        # Clear existing rows
        for row in self.tree.get_children():
            self.tree.delete(row)

        # Insert rows for each settings instance
        for settings in self.channel_settings_list:
            row_values = [settings.values[col] for col in self.tree['columns']]
            self.tree.insert('', 'end', values=row_values)

    def edit_row(self, event):
        # Get the selected item
        selected_item = self.tree.selection()
        if not selected_item:
            messagebox.showwarning("Edit Row", "No row selected.")
            return
        
        item_id = self.tree.index(selected_item)
        settings = self.channel_settings_list[int(item_id)]

        # Open an edit dialog
        EditDialog(self, settings, self.update_row)

    def update_row(self, settings):
        # Update the Treeview with new values
        self.populate_treeview()

    def on_item_press(self, event):
        """Called when the user presses down on an item for dragging."""
        region = self.tree.identify_region(event.x, event.y)
        if region == 'cell':
            item = self.tree.identify_row(event.y)
            if item:
                self.dragged_item = item

    def on_item_drag(self, event):
        """Drag-and-drop logic can be implemented here if needed."""
        pass  # Currently, there is no specific dragging visual feedback.

    def on_item_release(self, event):
        """Called when the user releases the mouse button after dragging."""
        if self.dragged_item is not None:
            # Identify the new position
            target_item = self.tree.identify_row(event.y)
            if target_item and target_item != self.dragged_item:
                # Get the index of both items
                dragged_index = self.tree.index(self.dragged_item)
                target_index = self.tree.index(target_item)

                # Move the dragged item in the channel settings list
                self.channel_settings_list.insert(target_index, self.channel_settings_list.pop(dragged_index))

                # Refresh the Treeview to show updated order
                self.populate_treeview()

            # Reset dragged item
            self.dragged_item = None

    def set_channel_setting(self, id, key, value):
        """Set a specific channel setting value."""
        if 0 <= id < len(self.channel_settings_list):
            self.channel_settings_list[id].set(key, value)
            self.populate_treeview()
        else:
            raise IndexError("Channel ID out of range.")

    def get_channel_setting(self, id, key):
        """Get a specific channel setting value."""
        if 0 <= id < len(self.channel_settings_list):
            return self.channel_settings_list[id].get(key)
        else:
            raise IndexError("Channel ID out of range.")

    def addData(self, idx):
        self.channel_settings_list.append(ChannelSettings())
        
    def setData(self, row, channelSettings):
        """Set the ChannelSettings object for a specific row."""
        if 0 <= row < len(self.channel_settings_list):
            self.channel_settings_list[row] = channelSettings
            self.populate_treeview()
        else:
            raise IndexError("Row index out of range.")

    def getData(self, row):
        """Get the ChannelSettings object for a specific row."""
        if 0 <= row < len(self.channel_settings_list):
            return self.channel_settings_list[row]
        else:
            raise IndexError("Row index out of range.")

    def delData(self, row):
        """Delete the ChannelSettings object at a specific row."""
        if 0 <= row < len(self.channel_settings_list):
            del self.channel_settings_list[row]
            self.populate_treeview()
        else:
            raise IndexError("Row index out of range.")


class EditDialog:
    def __init__(self, parent, settings, callback):
        self.top = tk.Toplevel(parent)
        self.top.title(f"Edit Channel Settings")
        self.settings = settings
        self.callback = callback

        # Create entry fields for each setting
        self.entries = {}
        for idx, (key, value) in enumerate(settings.values.items()):
            label = tk.Label(self.top, text=key)
            label.grid(row=idx, column=0, padx=10, pady=5)

            if isinstance(value, bool):
                # Create a checkbox for boolean values
                var = tk.BooleanVar(value=value)
                checkbox = tk.Checkbutton(self.top, variable=var)
                checkbox.grid(row=idx, column=1, padx=10, pady=5)
                self.entries[key] = var
            else:
                # Create a spinbox for numeric values
                spinbox = tk.Spinbox(self.top, from_=0, to=65535, increment=1, width=10)  # Adjust the range as needed
                spinbox.delete(0, 'end')
                spinbox.insert(0, str(value))
                spinbox.grid(row=idx, column=1, padx=10, pady=5)
                self.entries[key] = spinbox

        # Save and Cancel buttons
        save_button = tk.Button(self.top, text="Save", command=self.save)
        save_button.grid(row=len(settings.values), column=0, padx=10, pady=10)

        cancel_button = tk.Button(self.top, text="Cancel", command=self.top.destroy)
        cancel_button.grid(row=len(settings.values), column=1, padx=10, pady=10)

    def save(self):
        for key, entry in self.entries.items():
            if isinstance(entry, tk.BooleanVar):
                # Get the boolean value from the checkbox
                self.settings.set(key, entry.get())
            else:
                # Get the integer value from the spinbox
                self.settings.set(key, int(entry.get()))

        self.callback(self.settings)  # Notify the manager to update the treeview
        self.top.destroy()


if __name__ == "__main__":
    root = tk.Tk()
    root.title("Channel Settings Manager")
    app = ChannelSettingsTable(root)
    app.pack(fill='both', expand=True)  # Ensure the frame takes up space
    root.mainloop()
