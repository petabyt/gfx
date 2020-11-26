#include <stdio.h>
#include "gfx.h"

int main() {
	// Open a new window for drawing.
	struct gfx_window window = gfx_open(500, 500, "Testing example");


	// Set the current drawing color to green.
	gfx_setColor(&window, 255, 0, 0);
	gfx_pixel(&window, 10, 10);
	
	while(1) {
		// Wait for the user to press a character.
		struct gfx_interaction ia = gfx_event();

		if (ia.type == KEY) {
			// Quit if it is the letter q.
			if (ia.value == 'q') {
				break;
			} else {
				printf("You pressed: %c\n", ia.value);
			}	
		} else if (ia.type == MOUSE) {
			printf("Click: %d: [%d, %d]\n", ia.value, ia.coord[0], ia.coord[1]);
		}
	}
	return 0;
}
