import tkinter as tk
from PIL import Image, ImageTk
import os
import json

# ... existing imports and class definition ...

class StatusPanelCompositeWidget(tk.Canvas):
    def __init__(self, parent, layout_json_path, edit_mode=False):
        self.layout_json_path = layout_json_path
        self.edit_mode = edit_mode
        self.draw_grid = False
        # Load layout from JSON
        with open(layout_json_path, "r") as f:
            layout = json.load(f)

        panel_path = layout["panel_image"]
        indicators = layout["indicators"]

        # Load panel background (with alpha) at its native size
        self.panel_img = Image.open(panel_path).convert("RGBA")
        self.width, self.height = self.panel_img.size
        super().__init__(parent, width=self.width, height=self.height, highlightthickness=0)

        # Load indicator images, positions, and scales from JSON
        self.indicator_imgs = {}
        self.positions = {}
        self.scales = {}
        for key, info in indicators.items():
            pos = tuple(info["position"])
            scale = info.get("scale", 1.0)
            self.positions[key] = pos
            self.scales[key] = scale
            if "image_up" in info and "image_down" in info:
                img_up = Image.open(info["image_up"]).convert("RGBA") if os.path.exists(info["image_up"]) else None
                img_down = Image.open(info["image_down"]).convert("RGBA") if os.path.exists(info["image_down"]) else None
                self.indicator_imgs[key] = {"up": img_up, "down": img_down}
            elif "image" in info:
                img_path = info["image"]
                if os.path.exists(img_path):
                    img = Image.open(img_path).convert("RGBA")
                    self.indicator_imgs[key] = img
                else:
                    self.indicator_imgs[key] = None
            else:
                self.indicator_imgs[key] = None

        self.indicator_state = {k: False for k in self.positions}
        self.composited_img = None
        self.tk_img = None
        self.image_id = None
        
        # Always initialize drag variables
        self.dragging = False
        self.current_key = None
        self.start_x = 0
        self.start_y = 0

        # Add save button only in edit mode
        if self.edit_mode:
            self.save_button = tk.Button(parent, text="Save Positions", command=self.save_positions)
            self.save_button.pack(side=tk.BOTTOM, pady=5)

            # Bind mouse events only in edit mode
            self.bind("<Button-1>", self.start_drag)
            self.bind("<B1-Motion>", self.drag)
            self.bind("<ButtonRelease-1>", self.stop_drag)
            self.bind("<MouseWheel>", self.scale_dragged_indicator)  # Windows
            self.bind("<Button-4>", self.scale_dragged_indicator)    # Linux scroll up
            self.bind("<Button-5>", self.scale_dragged_indicator)    # Linux scroll down

        self.draw_panel()

    def start_drag(self, event):
        # Find which indicator was clicked
        for key, pos in self.positions.items():
            scale = self.scales.get(key, 1.0)
            img = None
            if isinstance(self.indicator_imgs[key], dict):
                # Rocker: use current state to get correct image
                state = self.indicator_state[key]
                img = self.indicator_imgs[key]["up"] if state else self.indicator_imgs[key]["down"]
            else:
                img = self.indicator_imgs[key]
            if img:
                w, h = int(img.width * scale), int(img.height * scale)
                x, y = pos
                if (x <= event.x <= x + w and 
                    y <= event.y <= y + h):
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

    def scale_dragged_indicator(self, event):
        if self.dragging and self.current_key:
            key = self.current_key
            # MouseWheel event: event.delta (Windows), Button-4/5: event.num (Linux)
            delta = 0
            if hasattr(event, 'delta') and event.delta:
                delta = event.delta
            elif hasattr(event, 'num'):
                if event.num == 4:
                    delta = 120
                elif event.num == 5:
                    delta = -120
            # Adjust scale
            scale = self.scales.get(key, 1.0)
            if delta > 0:
                scale *= 1.1
            elif delta < 0:
                scale /= 1.1
            scale = max(0.1, min(5.0, scale))  # Clamp scale
            self.scales[key] = scale
            self.draw_panel()

    def save_positions(self):
        # Load current layout
        with open(self.layout_json_path, "r") as f:
            layout = json.load(f)
        
        # Update positions and scales
        for key, pos in self.positions.items():
            layout["indicators"][key]["position"] = list(pos)
            layout["indicators"][key]["scale"] = self.scales.get(key, 1.0)
        
        # Save updated layout
        with open(self.layout_json_path, "w") as f:
            json.dump(layout, f, indent=4)
        
        print("Positions and scales saved successfully!")

    def draw_panel(self):
        base = self.panel_img.copy()
        # Draw grid in edit mode
        if self.edit_mode and self.draw_grid:
            import PIL.ImageDraw
            draw = PIL.ImageDraw.Draw(base)
            grid_spacing = 50
            w, h = base.size
            for x in range(0, w, grid_spacing):
                draw.line([(x, 0), (x, h)], fill=(180, 180, 180, 80), width=1)
            for y in range(0, h, grid_spacing):
                draw.line([(0, y), (w, y)], fill=(180, 180, 180, 80), width=1)
        for key, state in self.indicator_state.items():
            x, y = self.positions[key]
            scale = self.scales.get(key, 1.0)
            if isinstance(self.indicator_imgs[key], dict):
                # Rocker: always draw, use up or down image based on state
                img = self.indicator_imgs[key]["up"] if state else self.indicator_imgs[key]["down"]
                if img:
                    w, h = int(img.width * scale), int(img.height * scale)
                    img_resized = img.resize((w, h), Image.LANCZOS)
                    base.alpha_composite(img_resized, (x, y))
            elif state and self.indicator_imgs[key]:
                img = self.indicator_imgs[key]
                w, h = int(img.width * scale), int(img.height * scale)
                img_resized = img.resize((w, h), Image.LANCZOS)
                base.alpha_composite(img_resized, (x, y))
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
                if isinstance(self.indicator_imgs[k], dict):
                    # Toggle rocker state each time
                    self.indicator_state[k] = not self.indicator_state[k]
                else:
                    self.indicator_state[k] = True
            self.draw_panel()
            self.after(interval, step)
        step()

    def set_edit_mode(self, enabled):
        """Enable or disable edit mode after initialization"""
        if enabled != self.edit_mode:
            self.edit_mode = enabled
            if enabled:
                # Add save button
                self.save_button = tk.Button(self.master, text="Save Positions", command=self.save_positions)
                self.save_button.pack(side=tk.BOTTOM, pady=5)
                # Bind mouse events
                self.bind("<Button-1>", self.start_drag)
                self.bind("<B1-Motion>", self.drag)
                self.bind("<ButtonRelease-1>", self.stop_drag)
                self.bind("<MouseWheel>", self.scale_dragged_indicator)  # Windows
                self.bind("<Button-4>", self.scale_dragged_indicator)    # Linux scroll up
                self.bind("<Button-5>", self.scale_dragged_indicator)    # Linux scroll down
            else:
                # Remove save button
                if hasattr(self, 'save_button'):
                    self.save_button.destroy()
                # Unbind mouse events
                self.unbind("<Button-1>")
                self.unbind("<B1-Motion>")
                self.unbind("<ButtonRelease-1>")
                self.unbind("<MouseWheel>")
                self.unbind("<Button-4>")
                self.unbind("<Button-5>")

def main():
    root = tk.Tk()
    root.title("Status Panel Composite Widget Test")
    layout_json_path = "panel_layout.json"
    widget = StatusPanelCompositeWidget(root, layout_json_path)
    widget.set_edit_mode(True)
    widget.pack()
    widget.animate_indicators(interval=1000)
    root.mainloop()

if __name__ == "__main__":
    main()