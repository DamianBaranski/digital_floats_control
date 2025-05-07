import tkinter as tk
from PIL import Image, ImageTk
import os
import json

# ... existing imports and class definition ...

class StatusPanelCompositeWidget(tk.Canvas):
    def __init__(self, parent, layout_json_path):
        self.layout_json_path = layout_json_path
        # Load layout from JSON
        with open(layout_json_path, "r") as f:
            layout = json.load(f)

        panel_path = layout["panel_image"]
        indicators = layout["indicators"]

        # Load panel background (with alpha) at its native size
        self.panel_img = Image.open(panel_path).convert("RGBA")
        self.width, self.height = self.panel_img.size
        super().__init__(parent, width=self.width, height=self.height, highlightthickness=0)

        # Load indicator images and positions from JSON
        self.indicator_imgs = {}
        self.positions = {}
        for key, info in indicators.items():
            img_path = info["image"]
            pos = tuple(info["position"])
            self.positions[key] = pos
            if os.path.exists(img_path):
                img = Image.open(img_path).convert("RGBA")
                self.indicator_imgs[key] = img
            else:
                self.indicator_imgs[key] = None

        self.indicator_state = {k: False for k in self.positions}
        self.composited_img = None
        self.tk_img = None
        self.image_id = None
        
        # Add save button
        self.save_button = tk.Button(parent, text="Save Positions", command=self.save_positions)
        self.save_button.pack(side=tk.BOTTOM, pady=5)

        # Drag and drop variables
        self.dragging = False
        self.current_key = None
        self.start_x = 0
        self.start_y = 0

        # Bind mouse events
        self.bind("<Button-1>", self.start_drag)
        self.bind("<B1-Motion>", self.drag)
        self.bind("<ButtonRelease-1>", self.stop_drag)

        self.draw_panel()

    def start_drag(self, event):
        # Find which indicator was clicked
        for key, pos in self.positions.items():
            if self.indicator_imgs[key]:
                img = self.indicator_imgs[key]
                x, y = pos
                if (x <= event.x <= x + img.width and 
                    y <= event.y <= y + img.height):
                    self.dragging = True
                    self.current_key = key
                    self.start_x = event.x - x
                    self.start_y = event.y - y
                    break

    def drag(self, event):
        if self.dragging and self.current_key:
            new_x = event.x - self.start_x
            new_y = event.y - self.start_y
            self.positions[self.current_key] = (new_x, new_y)
            self.draw_panel()

    def stop_drag(self, event):
        self.dragging = False
        self.current_key = None

    def save_positions(self):
        # Load current layout
        with open(self.layout_json_path, "r") as f:
            layout = json.load(f)
        
        # Update positions
        for key, pos in self.positions.items():
            layout["indicators"][key]["position"] = list(pos)
        
        # Save updated layout
        with open(self.layout_json_path, "w") as f:
            json.dump(layout, f, indent=4)
        
        print("Positions saved successfully!")

    def draw_panel(self):
        base = self.panel_img.copy()
        for key, state in self.indicator_state.items():
            if state and self.indicator_imgs[key]:
                x, y = self.positions[key]
                ind_img = self.indicator_imgs[key]
                base.alpha_composite(ind_img, (x, y))
        self.composited_img = base
        self.tk_img = ImageTk.PhotoImage(self.composited_img)
        if self.image_id is None:
            self.image_id = self.create_image(0, 0, anchor=tk.NW, image=self.tk_img)
        else:
            self.itemconfig(self.image_id, image=self.tk_img)

    def set_indicator(self, key, state=True):
        if key in self.indicator_state:
            self.indicator_state[key] = state
            self.draw_panel()

    def toggle_indicator(self, key):
        if key in self.indicator_state:
            self.indicator_state[key] = not self.indicator_state[key]
            self.draw_panel()

    def animate_indicators(self, interval=500):
        keys = list(self.indicator_state.keys())
        import itertools
        cycle = itertools.cycle(keys)
        def step():
            for k in keys:
                self.indicator_state[k] = True
            k = next(cycle)
            self.indicator_state[k] = True
            self.draw_panel()
            self.after(interval, step)
        step()

def main():
    root = tk.Tk()
    root.title("Status Panel Composite Widget Test")
    layout_json_path = "panel_layout.json"
    widget = StatusPanelCompositeWidget(root, layout_json_path)
    widget.pack()
    widget.animate_indicators(interval=1000)
    root.mainloop()

if __name__ == "__main__":
    main()