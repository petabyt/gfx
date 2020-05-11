#include <stdio.h>
#include "gfx.h"

int main() {
	char c;

	// Open a new window for drawing.
	gfxOpen(500 ,500, "Testing example");

	// Set the current drawing color to green.
	gfxColor(0, 50, 50);

	// Draw a triangle on the screen.
	gfxLine(100, 100, 200, 100);
	gfxLine(200, 100, 150, 150);
	gfxLine(150, 150, 100, 100);

	while(1) {
		// Wait for the user to press a character.
		c = gfxWait();

		// Quit if it is the letter q.
		if(c == 'q') break;
	}

	return 0;
}
