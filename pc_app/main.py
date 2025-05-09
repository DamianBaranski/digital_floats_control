from core.app import DigitalFloatsApp
import tkinter as tk

def main():
    root = tk.Tk()
    root.geometry("1400x900")
    app = DigitalFloatsApp(root)
    app.grid(row=0, column=0, sticky="nsew")
    root.grid_rowconfigure(0, weight=1)
    root.grid_columnconfigure(0, weight=1)
    root.mainloop()

if __name__ == "__main__":
    main()
