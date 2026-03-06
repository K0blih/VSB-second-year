import time
import tkinter


class Colors:
    BLUE = 'blue'
    RED = 'red'
    GREEN = 'green'
    BLACK = 'black'
    YELLOW = 'yellow'


class PlaygroundWindow:

    def __init__(self, window_size):
        self.window_size = window_size

        self.master = tkinter.Tk()
        self.master.title('Playground')
        self.master.protocol("WM_DELETE_WINDOW", lambda: self.master.destroy())
        self.master.bind('<Escape>', lambda e: self.master.destroy())

        self.canvas = tkinter.Canvas(
            self.master, width=window_size[0], height=window_size[1], background=Colors.BLACK)
        self.canvas.pack()

        self.left_mouse_click = False
        self.right_mouse_click = False
        self.mouse_event_pos = (0, 0,)
        self.gravity_point_enabled = False
        self.gravity_point = (0, 0,)

        # Bind left mouse click
        self.canvas.bind('<Button-1>', self.on_left_click)
        self.canvas.bind('<Button-3>', self.on_right_click)

    def on_left_click(self, event):
        """Handles left mouse button click on the canvas."""
        print(f"Left click at: ({event.x}, {event.y})")
        self.left_mouse_click = True
        self.mouse_event_pos = (event.x, event.y,)
        #self.gravity_point_enabled = True
        #self.gravity_point = (event.x, event.y)



    def on_right_click(self, event):
        """Handles right mouse button click on the canvas."""
        print(f"Right click at: ({event.x}, {event.y})")
        self.right_mouse_click = True
        self.mouse_event_pos = (event.x, event.y,)
        #self.gravity_point_enabled = False

    def put_oval_to_canvas(self, atom):
        pos_x, pos_y, rad, col = atom
        o = self.canvas.create_oval(
            pos_x - rad, pos_y - rad, pos_x + rad, pos_y + rad, fill=col)

        return o

    def update(self):
        self.master.update()

    def delete_item_from_canvas(self, item):
        self.canvas.delete(item)

    def get_window_size(self):
        return (self.width, self.height,)


def run(size, world):
    playground_window = PlaygroundWindow(size)

    while True:
        coords = world.tick()

        ovals = []
        for coord in coords:
            oval = playground_window.put_oval_to_canvas(coord)
            ovals.append(oval)

        if playground_window.left_mouse_click:
            playground_window.left_mouse_click = False

            world.add_atom(*playground_window.mouse_event_pos)

        if playground_window.right_mouse_click:
            playground_window.right_mouse_click = False

            world.add_falldown_atom(*playground_window.mouse_event_pos)

        playground_window.update()

        time.sleep(0.1)

        for oval in ovals:
            playground_window.delete_item_from_canvas(oval)
