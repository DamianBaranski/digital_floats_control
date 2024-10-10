import tkinter as tk
from tkinter import colorchooser
from tkinter import ttk
from .user_settings import UserSettings
from .channel_settings import ChannelSettings
from .channel_settings_table import ChannelSettingsTable

class UserSettingField(tk.Frame):
    def __init__(self, parent, name, input_type="entry"):
        tk.Frame.__init__(self, parent)
        self.name = name
        self.label = tk.Label(self, text=f"{name}:")
        self.label.pack(side="left", padx=5, pady=5)

        if input_type == "entry":
            self.entry = tk.Entry(self)
            self.entry.pack(side="left", padx=5, pady=5, fill='x')
        elif input_type == "color":
            self.color_button = tk.Button(self, text="Choose Color", command=self.choose_color)
            self.color_button.pack(side="left", padx=5, pady=5)
            self.color_display = tk.Label(self, width=10, bg="white", relief="sunken")
            self.color_display.pack(side="left", padx=5, pady=5)

        self.input_type = input_type

    def choose_color(self):
        # Open color chooser dialog and set color
        color = colorchooser.askcolor(color=self.color_display.cget("bg"))[1]
        if color:
            self.color_display.config(bg=color)

    def set_value(self, value):
        if self.input_type == "entry":
            self.entry.delete(0, tk.END)
            self.entry.insert(0, value)
        elif self.input_type == "color":
            self.color_display.config(bg=value)

    def get_value(self):
        if self.input_type == "entry":
            return self.entry.get()
        elif self.input_type == "color":
            return self.color_display.cget("bg")

class SettingsFrameWidget(tk.Frame):
    def __init__(self, parent, protocol):
        tk.Frame.__init__(self, parent)
        self.user_settings = UserSettings()
        self.protocol = protocol
        # Frame for Channel Settings
        channel_frame = tk.Frame(self)
        channel_frame.pack(side="top", fill="x", padx=10, pady=10)

        self.channels_settings_label = tk.Label(channel_frame, text="Channels Settings", font=("Helvetica", 12, "bold"))
        self.channels_settings_label.pack(side="top", fill="x")
        
        self.channel_settings_table = ChannelSettingsTable(channel_frame)
        self.channel_settings_table.pack(side="top", fill="x")

        # Frame for Buttons (Load, Save, Autodetect)
        button_frame = tk.Frame(channel_frame)
        button_frame.pack(side="top", fill="x", pady=10)

        self.channel_settings_load = tk.Button(button_frame, text="Load", command=self.loadChannelSettings)
        self.channel_settings_load.pack(side="right", padx=5)

        self.channel_settings_save = tk.Button(button_frame, text="Save", command=self.saveChannelSettings)
        self.channel_settings_save.pack(side="right", padx=5)

        self.channel_settings_autodetect = tk.Button(button_frame, text="Autodetect", command=self.autoDetect)
        self.channel_settings_autodetect.pack(side="right", padx=5)
        
        self.channel_settings_help = tk.Button(button_frame, text="Help", command=self.channelHelp)
        self.channel_settings_help.pack(side="right", padx=5)

        # Frame for User Settings
        user_frame = tk.Frame(self)
        user_frame.pack(side="top", fill="x", padx=10, pady=10)

        self.user_settings_label = tk.Label(user_frame, text="User Settings", font=("Helvetica", 12, "bold"))
        self.user_settings_label.pack(side="top", fill="x")

        # Create a frame to group user setting fields
        user_settings_group = tk.Frame(user_frame)
        user_settings_group.pack(side="top", fill="x", pady=(5, 10))

        # Add user setting fields
        self.brightness = UserSettingField(user_settings_group, "Brightness")
        self.ldg_up_color = UserSettingField(user_settings_group, "LDG Up Color", input_type="color")
        self.ldg_down_color = UserSettingField(user_settings_group, "LDG Down Color", input_type="color")
        self.rudder_up_color = UserSettingField(user_settings_group, "Rudder Up Color", input_type="color")
        self.rudder_down_color = UserSettingField(user_settings_group, "Rudder Down Color", input_type="color")
        self.rudder_inactive_color = UserSettingField(user_settings_group, "Rudder Inactive Color", input_type="color")
        self.warning_color = UserSettingField(user_settings_group, "Warning Color", input_type="color")
        self.error_color = UserSettingField(user_settings_group, "Error Color", input_type="color")


        self.brightness.pack(side="top", fill="x", pady=5)
        self.ldg_up_color.pack(side="top", fill="x", pady=5)
        self.ldg_down_color.pack(side="top", fill="x", pady=5)
        self.rudder_up_color.pack(side="top", fill="x", pady=5)
        self.rudder_down_color.pack(side="top", fill="x", pady=5)
        self.rudder_inactive_color.pack(side="top", fill="x", pady=5)
        self.warning_color.pack(side="top", fill="x", pady=5)
        self.error_color.pack(side="top", fill="x", pady=5)

        # Add Save and load buttons
        button_frame = tk.Frame(self)
        button_frame.pack(side="top", fill="x", padx=10, pady=(5, 10))

        self.load_button = tk.Button(button_frame, text="Load", command=self.loadUserSettings)
        self.load_button.pack(side="right", padx=5)

        self.save_button = tk.Button(button_frame, text="Save", command=self.saveUserSettings)
        self.save_button.pack(side="right", padx=5)

        self.default_button = tk.Button(button_frame, text="Load Default", command=self.loadDefaultSettings)
        self.default_button.pack(side="right", padx=5)
        
        self.updateUserSettings()  
        
    def updateUserSettings(self):
        # Set values from UserSettings to each field
        self.brightness.set_value(self.user_settings.get('brightness'))
        self.ldg_up_color.set_value(self.user_settings.get('ldg_up_color'))
        self.ldg_down_color.set_value(self.user_settings.get('ldg_down_color'))
        self.rudder_up_color.set_value(self.user_settings.get('rudder_up_color'))
        self.rudder_down_color.set_value(self.user_settings.get('rudder_down_color'))
        self.rudder_inactive_color.set_value(self.user_settings.get('rudder_inactive_color'))
        self.warning_color.set_value(self.user_settings.get('warning_color'))
        self.error_color.set_value(self.user_settings.get('error_color'))

    def loadUserSettings(self):
        def callback(data):
            self.user_settings = data
            self.updateUserSettings()
    
        self.protocol.getUserSettings(callback)
        

    def saveUserSettings(self):
        # Save the current values from fields to UserSettings
        self.user_settings.set('brightness', self.brightness.get_value())
        self.user_settings.set('ldg_up_color', self.ldg_up_color.get_value())
        self.user_settings.set('ldg_down_color', self.ldg_down_color.get_value())
        self.user_settings.set('rudder_up_color', self.rudder_up_color.get_value())
        self.user_settings.set('rudder_down_color', self.rudder_down_color.get_value())
        self.user_settings.set('rudder_inactive_color', self.rudder_inactive_color.get_value())
        self.user_settings.set('warning_color', self.warning_color.get_value())
        self.user_settings.set('error_color', self.error_color.get_value())
        self.protocol.updateUserSettings(self.user_settings)        

    def loadDefaultSettings(self):
        # Load default settings
        self.user_settings.setDefaults()
        self.updateUserSettings()   

            
    def autoDetect(self):
        ina_addr = []
        pcf_addr = []
        old_settings = []
        new_settings = []
        
        def detect_ina():
            print("Detect ina addr")
            ina_start_address = 0x40
            max_no_ina_sensors = 16
            def ina_callback(data, address):
                ina_addr.append((address, data))
                print("ina callback:", (address, data))
                
                if len(ina_addr) >= max_no_ina_sensors:
                    print("call detect pcf:", len(ina_addr), max_no_ina_sensors)
                    detect_pcf()
                else:
                    self.protocol.scanI2c(address+1, lambda data, addr=address+1: ina_callback(data, addr))
            
            self.protocol.scanI2c(ina_start_address, lambda data, addr=ina_start_address: ina_callback(data, addr))

        def detect_pcf():
            pcf_start_address = 0x20
            max_no_pcf_expanders = 8
        
            def pcf_callback(data, address):
                pcf_addr.append((address, data))
                print("pcf callback:", (address, data))
                if len(pcf_addr) == max_no_pcf_expanders:
                    print("call get settings:", len(pcf_addr), max_no_pcf_expanders)
                    getSettings()
            
            for i in range(pcf_start_address, pcf_start_address+max_no_pcf_expanders):
                self.protocol.scanI2c(i, lambda data, addr=i: pcf_callback(data, addr))
        
        def getSettings():
            print("Get settings")
            status = [False] * 6
            def callback(channel_settings, idx):
                print("Get settings callback:", idx)
                status[idx] = True
                old_settings.append(channel_settings)
                if all(status):
                    clean_settings()

            for i in range(6):
                self.protocol.getChannelSettings(i, lambda data, idx=i: callback(data, idx))
        
        def clean_settings():
            print("Disable channels")
            status = [False] * 6
            def callback(ret, idx):
                print("Disable channels callback:", idx)
                status[idx] = True
                if all(status):
                    print("Call test relays")
                    test_relays()

            for i in range(6):
                self.protocol.updateChannelSettings(i, ChannelSettings(), lambda ret, idx=i: callback(ret, idx))
        
        def disable_channels():
            status = [False]*len(pcf_addr)*2
            def callback(ret, idx, channel):
                status[idx*2+channel] = True
                if all(status):
                    test_relays()
            
            for (idx, addr) in pcf_addr:
                self.protocol.testRelays(addr, 0, 1, callback)
                self.protocol.testRelays(addr, 1, 1, callback)                

        def test_relays():
            print("Test relays")

            # Filter only those elements where the second item is True
            tmp_ina = [item[0] for item in ina_addr if item[1] == True]
            tmp_pcf = [item[0] for item in pcf_addr if item[1] == True]

            # Update ina_addr and pcf_addr with filtered values
            ina_addr[:] = tmp_ina
            pcf_addr[:] = tmp_pcf
            ina_idx = 0
            pcf_idx = 0
            pcf_channel = 0
            
            def next_pcf():
                print("Taking next pcf")
                nonlocal pcf_addr, ina_addr, pcf_channel, pcf_idx, ina_idx
                if pcf_channel == 0:
                    pcf_channel = 1
                else:
                    pcf_idx+=1
                    pcf_channel = 0
                ina_idx = 0

            def callback(ret):
                nonlocal pcf_addr, ina_addr, pcf_channel, pcf_idx, ina_idx

                print(f"test relay callback pcf_addr: {pcf_addr[pcf_idx]}  pcf_channel: {pcf_channel}  ina_addr: {ina_addr[ina_idx]}  ret: {ret}")

                # If the relay test returned True, save the settings and remove the INA address
                if ret:
                    settings = ChannelSettings()
                    settings.values['pcf_addr'] = pcf_addr[pcf_idx]
                    settings.values['pcf_channel'] = pcf_channel
                    settings.values['ina_addr'] = ina_addr[ina_idx]
                    new_settings.append(settings)

                    # Remove the current INA address after successful test
                    ina_addr.pop(ina_idx)
                    # Reset to test the next INA address from the start
                    next_pcf()

                else:
                    # If the test returns false, increment the INA index
                    ina_idx += 1

                # Check if we have finished testing all INA addresses for the current PCF channel
                if ina_idx >= len(ina_addr):
                    # Move to the next PCF channel
                    next_pcf()

                # Stop when all PCF addresses and INA addresses have been tested
                if pcf_idx >= len(pcf_addr):
                    print("All PCF addresses tested.")
                    for s in new_settings:
                        print(s)
                    generate_new_settings()
                    return

                # Continue testing with the next INA address or channel
                if pcf_idx < len(pcf_addr) and ina_idx < len(ina_addr):
                    print(f"Testing PCF address: {pcf_addr[pcf_idx]}, PCF channel: {pcf_channel}, INA address: {ina_addr[ina_idx]}")
                    self.protocol.testRelays(pcf_addr[pcf_idx], pcf_channel, ina_addr[ina_idx], callback)
                else:
                    print("No more INA addresses to test.")

            # Start the relay testing with the first INA address and first PCF address on channel 0
            print("ina_addr:", ina_addr)
            print("pcf_addr:", pcf_addr)

            if len(pcf_addr) > 0 and len(ina_addr) > 0:
                self.protocol.testRelays(pcf_addr[pcf_idx], pcf_channel, ina_addr[ina_idx], callback)
            else:
                print("No channels detected.")  

        def generate_new_settings():
            for (idx, s) in enumerate(new_settings):
                s.values['max_voltage_limit'] = 260
                s.values['min_voltage_limit'] = 80
                s.values['max_current_limit'] = 50
                s.values['min_current_limit'] = 0
                s.values['ina_calibration'] = 50
                s.values['enable'] = True
                self.channel_settings_table.addData(idx)
                self.channel_settings_table.setData(idx, s)
            self.channel_settings_table.populate_treeview()

        detect_ina()

    def channelHelp(self):
        self.channel_settings_table.display_instructions()

    def loadChannelSettings(self):
        status = [False] * 6
        def callback(channel_settings, idx):
            status[idx] = True
            self.channel_settings_table.addData(idx)
            self.channel_settings_table.setData(idx, channel_settings) 
            if all(status):
                self.channel_settings_table.populate_treeview()

        for i in range(6):
            self.protocol.getChannelSettings(i, lambda data, idx=i: callback(data, idx))

    def saveChannelSettings(self):
        for i in range(6):

            channel_settings = self.channel_settings_table.getData(i)
            self.protocol.updateChannelSettings(i, channel_settings)
            