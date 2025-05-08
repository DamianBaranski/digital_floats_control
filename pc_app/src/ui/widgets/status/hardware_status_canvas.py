import tkinter as tk
from PIL import Image, ImageTk
import os
import json
try:
    from src.ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, FONT, HEADER_FONT
except ImportError:
    # Fallback for running as a script: define defaults
    DARK_BG = "#23272a"
    DARKER_BG = "#2d333b"
    BORDER_COLOR = "#36393f"
    TEXT_COLOR = "#fff"
    SECONDARY_TEXT = "#aaa"
    FONT = ("Segoe UI", 10)
    HEADER_FONT = ("Segoe UI", 14, "bold")

# ... existing imports and class definition ...

def make_white_bg_transparent(img, tolerance=190):
    img = img.convert("RGBA")
    datas = img.getdata()
    new_data = []
    for item in datas:
        # Calculate distance from white
        dist = ((item[0] - 255) ** 2 + (item[1] - 255) ** 2 + (item[2] - 255) ** 2) ** 0.5
        if dist < tolerance:
            new_data.append((255, 255, 255, 0))
        else:
            new_data.append(item)
    img.putdata(new_data)
    return img

class HardwareStatusCanvas(tk.Canvas):
    def __init__(self, parent, layout_json_path, edit_mode=False):
        self.layout_json_path = layout_json_path
        self.edit_mode = edit_mode
        self.draw_grid = False
        # Always resolve resource paths relative to this script
        base_dir = os.path.dirname(os.path.abspath(__file__))
        layout_json_path = os.path.join(base_dir, layout_json_path) if not os.path.isabs(layout_json_path) else layout_json_path
        self.layout_json_path = layout_json_path
        # Load layout from JSON
        with open(layout_json_path, "r") as f:
            layout = json.load(f)

        panel_path = layout["panel_image"]
        if not os.path.isabs(panel_path):
            panel_path = os.path.join(os.path.dirname(layout_json_path), panel_path)
        self.panel_scale = layout.get("panel_scale", 1.0)
        indicators = layout["indicators"]

        # Load panel background (with alpha) at its native size, then scale
        self.panel_img_orig = Image.open(panel_path).convert("RGBA")
        orig_width, orig_height = self.panel_img_orig.size
        self.panel_img = self.panel_img_orig.resize((int(orig_width * self.panel_scale), int(orig_height * self.panel_scale)), Image.LANCZOS)
        self.width, self.height = self.panel_img.size
        super().__init__(parent, width=self.width, height=self.height, highlightthickness=0, bg=DARK_BG)
        self.bind('<Configure>', self._on_resize)
        self._last_drawn_size = (self.width, self.height)
        self._center_x = self.width // 2
        self._center_y = self.height // 2

        # Load indicator images, positions, and scales from JSON
        self.indicator_imgs = {}
        self.positions = {}
        self.scales = {}
        self.animated_bg_color = (255, 255, 255, 255)
        for key, info in indicators.items():
            pos = tuple(info["position"])
            scale = info.get("scale", 1.0)
            self.positions[key] = pos
            self.scales[key] = scale
            if "image_up" in info and "image_down" in info:
                img_up_path = info["image_up"]
                img_down_path = info["image_down"]
                if not os.path.isabs(img_up_path):
                    img_up_path = os.path.join(base_dir, img_up_path)
                if not os.path.isabs(img_down_path):
                    img_down_path = os.path.join(base_dir, img_down_path)
                img_up = Image.open(img_up_path).convert("RGBA") if os.path.exists(img_up_path) else None
                img_down = Image.open(img_down_path).convert("RGBA") if os.path.exists(img_down_path) else None
                self.indicator_imgs[key] = {"up": img_up, "down": img_down}
            elif "image" in info:
                img_path = info["image"]
                if not os.path.isabs(img_path):
                    img_path = os.path.join(base_dir, img_path)
                if os.path.exists(img_path):
                    img = Image.open(img_path).convert("RGBA")
                    # For RR, RL, FL, FR, RUDDER: preprocess to make white bg transparent
                    if key in ("RR", "RL", "FL", "FR", "RUDDER"):
                        img = make_white_bg_transparent(img)
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
            self.save_button = tk.Button(parent, text="Save Positions", command=self.save_positions,
                                       bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                       activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
            self.save_button.pack(side=tk.BOTTOM, pady=5)

            # Bind mouse events only in edit mode
            self.bind("<Button-1>", self.start_drag)
            self.bind("<B1-Motion>", self.drag)
            self.bind("<ButtonRelease-1>", self.stop_drag)
            self.bind("<MouseWheel>", self.scale_dragged_indicator)  # Windows
            self.bind("<Button-4>", self.scale_dragged_indicator)    # Linux scroll up
            self.bind("<Button-5>", self.scale_dragged_indicator)    # Linux scroll down

        self.draw_panel()

    def _on_resize(self, event):
        canvas_w = event.width
        canvas_h = event.height
        orig_w, orig_h = self.panel_img_orig.size
        scale_w = canvas_w / orig_w
        scale_h = canvas_h / orig_h
        scale = min(scale_w, scale_h)
        if self._last_drawn_size != (canvas_w, canvas_h):
            self._center_x = canvas_w // 2
            self._center_y = canvas_h // 2
            self.set_panel_scale(scale)
            self._last_drawn_size = (canvas_w, canvas_h)

    def set_panel_scale(self, new_scale, center_x=None, center_y=None):
        self.panel_scale = new_scale
        orig_width, orig_height = self.panel_img_orig.size
        self.panel_img = self.panel_img_orig.resize((int(orig_width * self.panel_scale), int(orig_height * self.panel_scale)), Image.LANCZOS)
        self.width, self.height = self.panel_img.size
        self.config(width=self.width, height=self.height)
        if center_x is not None and center_y is not None:
            self._center_x = center_x
            self._center_y = center_y
        self.draw_panel()

    def start_drag(self, event):
        # Find which indicator was clicked
        for key, pos in self.positions.items():
            scale = self.scales.get(key, 1.0) * self.panel_scale
            img = None
            if isinstance(self.indicator_imgs[key], dict):
                # Rocker: use current state to get correct image
                state = self.indicator_state[key]
                img = self.indicator_imgs[key]["up"] if state else self.indicator_imgs[key]["down"]
            else:
                img = self.indicator_imgs[key]
            if img:
                # Convert stored position to display position
                x_disp, y_disp = int(pos[0] * self.panel_scale), int(pos[1] * self.panel_scale)
                w, h = int(img.width * scale), int(img.height * scale)
                if (x_disp <= event.x <= x_disp + w and 
                    y_disp <= event.y <= y_disp + h):
                    self.dragging = True
                    self.current_key = key
                    # Store offset in display space, but convert to unscaled for updating
                    self.start_x = (event.x - x_disp) / self.panel_scale
                    self.start_y = (event.y - y_disp) / self.panel_scale
                    break

    def drag(self, event):
        if self.dragging and self.current_key:
            # Convert mouse position to unscaled/original space
            new_x = (event.x / self.panel_scale) - self.start_x
            new_y = (event.y / self.panel_scale) - self.start_y
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

    def draw_panel(self, center_x=None, center_y=None):
        base = self.panel_img.copy()
        # Draw grid in edit mode
        if self.edit_mode and self.draw_grid:
            import PIL.ImageDraw
            draw = PIL.ImageDraw.Draw(base)
            grid_spacing = int(50 * self.panel_scale)
            w, h = base.size
            for x in range(0, w, grid_spacing):
                draw.line([(x, 0), (x, h)], fill=(180, 180, 180, 80), width=1)
            for y in range(0, h, grid_spacing):
                draw.line([(0, y), (w, y)], fill=(180, 180, 180, 80), width=1)
        for key, state in self.indicator_state.items():
            # Scale position and size by panel_scale
            orig_x, orig_y = self.positions[key]
            x, y = int(orig_x * self.panel_scale), int(orig_y * self.panel_scale)
            scale = self.scales.get(key, 1.0) * self.panel_scale
            if isinstance(self.indicator_imgs[key], dict):
                # Rocker: always draw, use up or down image based on state
                img = self.indicator_imgs[key]["up"] if state else self.indicator_imgs[key]["down"]
                if img:
                    w, h = int(img.width * scale), int(img.height * scale)
                    img_resized = img.resize((w, h), Image.LANCZOS)
                    base.alpha_composite(img_resized, (x, y))
            elif key in ("RR", "RL", "FL", "FR", "RUDDER") and self.indicator_imgs[key]:
                # Composite over animated background color
                img = self.indicator_imgs[key]
                w, h = int(img.width * scale), int(img.height * scale)
                img_resized = img.resize((w, h), Image.LANCZOS)
                # Create a background of the current color
                bg = Image.new("RGBA", (w, h), self.animated_bg_color)
                bg.alpha_composite(img_resized, (0, 0))
                base.alpha_composite(bg, (x, y))
            elif state and self.indicator_imgs[key]:
                img = self.indicator_imgs[key]
                w, h = int(img.width * scale), int(img.height * scale)
                img_resized = img.resize((w, h), Image.LANCZOS)
                base.alpha_composite(img_resized, (x, y))
        self.composited_img = base
        self.tk_img = ImageTk.PhotoImage(self.composited_img)
        # Always use the last known center
        x = self._center_x - self.width // 2
        y = self._center_y - self.height // 2
        if self.image_id is None:
            self.image_id = self.create_image(x, y, anchor=tk.NW, image=self.tk_img)
        else:
            self.coords(self.image_id, x, y)
            self.itemconfig(self.image_id, image=self.tk_img)

    def set_indicator(self, key, state=True):
        if key in self.indicator_state:
            self.indicator_state[key] = state
            self.draw_panel()

    def toggle_indicator(self, key):
        if key in self.indicator_state:
            self.indicator_state[key] = not self.indicator_state[key]
            self.draw_panel()

    def animate_indicators_edit_mode(self, interval=500):
        keys = list(self.indicator_state.keys())
        import itertools
        import colorsys
        cycle = itertools.cycle(keys)
        color_hue = [0]  # mutable for closure
        def step():
            for k in keys:
                if isinstance(self.indicator_imgs[k], dict):
                    self.indicator_state[k] = not self.indicator_state[k]
                else:
                    self.indicator_state[k] = True
            # Animate background color for RR, RL, FL, FR
            h = (color_hue[0] % 360) / 360.0
            r, g, b = [int(x * 255) for x in colorsys.hsv_to_rgb(h, 1, 1)]
            self.animated_bg_color = (r, g, b, 255)
            color_hue[0] += 10
            self.draw_panel()
            self.after(interval, step)
        step()

    def set_edit_mode(self, enabled):
        """Enable or disable edit mode after initialization"""
        if enabled != self.edit_mode:
            self.edit_mode = enabled
            if enabled:
                # Add save button
                self.save_button = tk.Button(self.master, text="Save Positions", command=self.save_positions,
                                           bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                           activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
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
    layout_json_path = "res/panel_layout.json"
    widget = HardwareStatusCanvas(root, layout_json_path)
    widget.set_edit_mode(True)
    widget.pack()
    widget.animate_indicators_edit_mode(interval=1000)
    root.mainloop()

if __name__ == "__main__":
    main()