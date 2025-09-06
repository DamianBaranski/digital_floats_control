import tkinter as tk
from tkinter import ttk, messagebox
from src.core.datatypes.channel_settings import ChannelSettings
from ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, FONT


class ChannelSettingsTablePanel(tk.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        self.channel_settings_list = []
        self.header_tooltip = None
        self.create_treeview()
        self.dragged_item = None  # Track the item being dragged

    def create_treeview(self):
        # Create a frame for the Treeview and Scrollbars
        frame = tk.Frame(self)
        frame.pack(fill='both', expand=True)

        # Use a smaller font for compactness
        self.compact_font = ("TkDefaultFont", 8)

        # Create the Treeview
        self.tree = ttk.Treeview(frame, selectmode='browse')
        self.tree.pack(side='left', fill='both', expand=True)

        # No scrollbars

        # Abbreviated column names and tooltips (add channel number as first column)
        self.column_map = [
            ("ch", "channel"),
            ("en", "enable"),
            ("br", "bridge"),
            ("inv_m", "inverse_motor"),
            ("inv_up", "inverse_up_limit_switch"),
            ("inv_dn", "inverse_down_limit_switch"),
            ("inv_lim", "inverse_limit_switch"),
            ("rud", "rudder"),
            ("br_ch", "bridge_channel"),
            ("time", "timeout"),
            ("i_warn", "max_current_warning_limit"),
            ("i_err", "max_current_error_limit"),
            ("i_min", "min_current_limit")
        ]
        column_names = [abbr for abbr, full in self.column_map]
        self.tree['columns'] = column_names
        self.tree['show'] = "headings"

        # Create headings and set column widths
        for abbr, full in self.column_map:
            self.tree.heading(abbr, text=abbr)
            self.tree.column(abbr, anchor="center", width=35, minwidth=10, stretch=True)

        # Set font for all children widgets (including headings and cells)
        style = ttk.Style()
        style.configure("Treeview", font=self.compact_font, rowheight=12)
        style.configure("Treeview.Heading", font=self.compact_font)

        # Add tooltips for column headers (fixed)
        self.tree.bind('<Motion>', self._on_treeview_motion)
        self.tree.bind('<Leave>', self._on_treeview_leave)
        self.tree.bind('<Configure>', self._on_treeview_configure)

        # Insert multiple rows
        self.addData(6)
        self.populate_treeview()

        # Bind the double-click event to edit a row
        self.tree.bind('<Double-1>', self.edit_row)

        # Bind mouse events for drag and drop
        self.tree.bind('<ButtonPress-1>', self.on_item_press)
        self.tree.bind('<B1-Motion>', self.on_item_drag)
        self.tree.bind('<ButtonRelease-1>', self.on_item_release)

    def _on_treeview_motion(self, event):
        # Check if mouse is over a heading, and show tooltip if so
        region = self.tree.identify_region(event.x, event.y)
        if region == 'heading':
            col = self.tree.identify_column(event.x)
            col_index = int(col.replace('#', '')) - 1
            if 0 <= col_index < len(self.column_map):
                abbr, full = self.column_map[col_index]
                try:
                    x, y, width, height = self.tree.bbox('heading', abbr)
                except Exception:
                    self._hide_header_tooltip()
                    return
                if not self.header_tooltip:
                    self.header_tooltip = tk.Toplevel(self.tree)
                    self.header_tooltip.wm_overrideredirect(True)
                    self.header_tooltip.geometry(f"+{self.tree.winfo_rootx() + x}+{self.tree.winfo_rooty() + y + height}")
                    label = tk.Label(self.header_tooltip, text=full, bg="#ffffe0", relief="solid", borderwidth=1, font=self.compact_font)
                    label.pack()
                else:
                    # Move tooltip if already shown
                    self.header_tooltip.geometry(f"+{self.tree.winfo_rootx() + x}+{self.tree.winfo_rooty() + y + height}")
                    self.header_tooltip.children['!label'].config(text=full)
            else:
                self._hide_header_tooltip()
        else:
            self._hide_header_tooltip()

    def _on_treeview_leave(self, event):
        self._hide_header_tooltip()

    def _hide_header_tooltip(self):
        if self.header_tooltip:
            self.header_tooltip.destroy()
            self.header_tooltip = None

    def _on_treeview_configure(self, event):
        # Autofit columns to fill the Treeview width
        total_width = self.tree.winfo_width()
        n_cols = len(self.column_map)
        if n_cols == 0:
            return
        col_width = max(int(total_width / n_cols), 10)
        for abbr, _ in self.column_map:
            self.tree.column(abbr, width=col_width)

    def display_instructions(self):
        """Display instructions for editing and dragging items."""
        instruction_message = (
            "Instructions:\n"
            "- Double-click a row to edit the settings.\n"
            "- Click and drag a row to reorder.\n"
            "- Use the scrollbars to navigate.\n"
            "- Hover over column headers for full names."
        )
        messagebox.showinfo("Instructions", instruction_message)

    def populate_treeview(self):
        for row in self.tree.get_children():
            self.tree.delete(row)
        for idx, settings in enumerate(self.channel_settings_list):
            row_values = [idx + 1]  # Channel number starts from 1
            # Add values for each column (skip the first column which is channel number)
            for abbr, full in self.column_map[1:]:
                if full in settings.values:
                    value = settings.values[full]
                    # Convert boolean values to display format
                    if isinstance(value, bool):
                        row_values.append("✓" if value else "✗")
                    else:
                        row_values.append(str(value))
                else:
                    row_values.append("")  # Default empty value if field doesn't exist
            self.tree.insert('', 'end', values=row_values)

    def edit_row(self, event):
        # Get the selected item
        selected_item = self.tree.selection()
        if not selected_item:
            messagebox.showwarning("Edit Row", "No row selected.")
            return
        item_id = self.tree.index(selected_item)
        settings = self.channel_settings_list[int(item_id)]
        EditDialog(self, settings, self.update_row)

    def update_row(self, settings):
        self.populate_treeview()

    def on_item_press(self, event):
        region = self.tree.identify_region(event.x, event.y)
        if region == 'cell':
            item = self.tree.identify_row(event.y)
            if item:
                self.dragged_item = item

    def on_item_drag(self, event):
        pass

    def on_item_release(self, event):
        if self.dragged_item is not None:
            target_item = self.tree.identify_row(event.y)
            if target_item and target_item != self.dragged_item:
                dragged_index = self.tree.index(self.dragged_item)
                target_index = self.tree.index(target_item)
                self.channel_settings_list.insert(target_index, self.channel_settings_list.pop(dragged_index))
                self.populate_treeview()
            self.dragged_item = None

    def set_channel_setting(self, id, key, value):
        if 0 <= id < len(self.channel_settings_list):
            self.channel_settings_list[id].set(key, value)
            self.populate_treeview()
        else:
            raise IndexError("Channel ID out of range.")

    def get_channel_setting(self, id, key):
        if 0 <= id < len(self.channel_settings_list):
            return self.channel_settings_list[id].get(key)
        else:
            raise IndexError("Channel ID out of range.")

    def addData(self, idx):
        while len(self.channel_settings_list) < idx:
            self.channel_settings_list.append(ChannelSettings())
        
    def setData(self, row, channelSettings):
        print("Set data:", row)
        if 0 <= row < len(self.channel_settings_list):
            self.channel_settings_list[row] = channelSettings
        else:
            raise IndexError("Row index out of range.")

    def getData(self, row):
        if 0 <= row < len(self.channel_settings_list):
            return self.channel_settings_list[row]
        else:
            raise IndexError("Row index out of range.")

    def delData(self, row):
        if 0 <= row < len(self.channel_settings_list):
            del self.channel_settings_list[row]
            self.populate_treeview()
        else:
            raise IndexError("Row index out of range.")


class EditDialog:
    def __init__(self, parent, settings, callback):
        self.top = tk.Toplevel(parent)
        self.top.title(f"Edit Channel Settings")
        self.top.configure(bg=DARK_BG)
        self.settings = settings
        self.callback = callback
        self.entries = {}
        self.tooltip = None

        # Create a frame for better organization
        main_frame = tk.Frame(self.top, bg=DARK_BG)
        main_frame.pack(padx=20, pady=20, fill='both', expand=True)

        # Define tooltips for each setting
        self.tooltips = {
            'channel': 'Channel number',
            'enable': 'Enable/disable this channel',
            'bridge': 'Enable bridge mode for motor control',
            'inverse_motor': 'Invert the motor direction',
            'inverse_up_limit_switch': 'Invert the up limit switch logic',
            'inverse_down_limit_switch': 'Invert the down limit switch logic',
            'inverse_limit_switch': 'Invert both limit switches logic',
            'rudder': 'Configure channel as rudder control',
            'bridge_channel': 'Channel number for bridge mode',
            'timeout': 'Movement timeout in seconds',
            'max_current_warning_limit': 'Maximum current warning limit in A units',
            'max_current_error_limit': 'Maximum current error limit in A units',
            'min_current_limit': 'Minimum current limit in A units'
        }

        for idx, (key, value) in enumerate(settings.values.items()):
            # Create a frame for each row to handle tooltips
            row_frame = tk.Frame(main_frame, bg=DARK_BG)
            row_frame.grid(row=idx, column=0, columnspan=2, sticky='w', padx=10, pady=5)
            
            label = tk.Label(row_frame, text=key, bg=DARK_BG, fg=TEXT_COLOR, font=FONT)
            label.pack(side='left')
            
            # Bind tooltip events to the label
            label.bind('<Enter>', lambda e, k=key: self._show_tooltip(e, k))
            label.bind('<Leave>', self._hide_tooltip)
            
            if isinstance(value, bool):
                var = tk.BooleanVar(value=value)
                checkbox = tk.Checkbutton(row_frame, variable=var, 
                                        bg=DARK_BG, fg=TEXT_COLOR, 
                                        selectcolor=DARKER_BG,
                                        activebackground=BORDER_COLOR,
                                        activeforeground=TEXT_COLOR)
                checkbox.pack(side='left', padx=10)
                self.entries[key] = var
            else:
                spinbox = tk.Spinbox(row_frame, from_=0, to=65535, increment=1, width=10,
                                   bg=DARKER_BG, fg=TEXT_COLOR,
                                   insertbackground=TEXT_COLOR,
                                   buttonbackground=BORDER_COLOR,
                                   font=FONT)
                spinbox.delete(0, 'end')
                spinbox.insert(0, str(value))
                spinbox.pack(side='left', padx=10)
                self.entries[key] = spinbox

        # Button frame
        button_frame = tk.Frame(main_frame, bg=DARK_BG)
        button_frame.grid(row=len(settings.values), column=0, columnspan=2, pady=20)

        save_button = tk.Button(button_frame, text="Save", command=self.save,
                              bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                              activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        save_button.pack(side='left', padx=10)

        cancel_button = tk.Button(button_frame, text="Cancel", command=self.top.destroy,
                                bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        cancel_button.pack(side='left', padx=10)

    def _show_tooltip(self, event, key):
        if self.tooltip:
            self.tooltip.destroy()
        
        x, y, _, _ = event.widget.bbox("insert")
        x += event.widget.winfo_rootx() + 25
        y += event.widget.winfo_rooty() + 25

        self.tooltip = tk.Toplevel(self.top)
        self.tooltip.wm_overrideredirect(True)
        self.tooltip.wm_geometry(f"+{x}+{y}")
        
        label = tk.Label(self.tooltip, text=self.tooltips[key],
                        bg="#ffffe0", relief="solid", borderwidth=1,
                        font=FONT, padx=5, pady=2)
        label.pack()

    def _hide_tooltip(self, event):
        if self.tooltip:
            self.tooltip.destroy()
            self.tooltip = None

    def save(self):
        for key, entry in self.entries.items():
            if isinstance(entry, tk.BooleanVar):
                self.settings.set(key, entry.get())
            else:
                self.settings.set(key, int(entry.get()))
        self.callback(self.settings)
        self.top.destroy()
