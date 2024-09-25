class UserSettings:
    def __init__(self):
        self.values = {}
        self.setDefaults()
    
    def setDefaults(self):
        self.values['brightness'] = '128'
        self.values['ldg_up_color'] = '#0000FF'
        self.values['ldg_down_color'] = '#00FF00'
        self.values['rudder_up_color'] = '#00FF00'
        self.values['rudder_down_color'] = '#0000FF'
        self.values['rudder_inactive_color'] = '#FFFF00'
        self.values['warning_color'] = '#FFA500'
        self.values['error_color'] = '#FF0000'
        
    def get(self, key):
        return self.values.get(key, '')

    def set(self, key, value):
        self.values[key] = value