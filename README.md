# GFX
A simple C library for creating graphical windows.

```
int main() {
	// Open a new window for drawing.
	struct gfx_window window = gfx_open(500, 500, "Testing example");

	// Set the current drawing color to green.
	gfx_setColor(&window, 255, 0, 0);
	gfx_pixel(&window, 10, 10);
	
	while(1) {
		// Wait for the user to press a character.
		struct gfx_interaction ia = gfx_event();
		if (ia.type == KEY && ia.value == 'q') {
			break;
		}
	}
	
	return 0;
}
```

## Credits
Original code is from:  
https://www3.nd.edu/~dthain/courses/cse20211/fall2013/gfx/
"A simple graphics library for CSE 20211 by Douglas Thain"  
Attribution 4.0 International (CC BY 4.0)  
