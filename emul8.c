/* ESE 350, Spring 2018
 * ------------------
 * Lab Group 8
 * Francisco Masuelli (masuelli@seas.upenn.edu)
 * Arush Jain (arushj@seas.upenn.edu)
 */

#include <GL/glut.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "gamul.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

/* System state and status variables
 *  memory -> 4096 bytes (0xFFF : 0x000), first 512 bytes reserved for interpreter data, such as character and letter sprites (0x200 : 0x000)
 *  reg_Vx -> 16 general purpose 8-bit registers (Vx -> VF : V0), VF is reserved for instruction flag
 *  ID_reg -> 16-bit index register (I), used to store mem addrs for loading from/storing to mem -> (0xFFF : 0x000)
 *  keypad -> 16 keys (0xF : 0x0)
 *  stack -> 16 16-bit values, used to store addr that interpreter should ret to when finished w/a subroutine
 *  spntr -> Stack Pointer is 8-bit and points to topmost mem loc of the stack
 *  sound -> 8-bit when nonzero, decrement at a rate of 60Hz
 *  delay -> 8-bit; when nonzero, decrement at a rate of 60Hz
 *  PC -> 16 bit, sote the currently executing addr (0xFFF : 0x000)
 */
unsigned short *memory;
unsigned short *reg_Vx;
unsigned short *ID_reg;
unsigned short *keypad;
unsigned short *stack;
unsigned short *spntr;
unsigned short *sound;
unsigned short *delay;
unsigned short *PC;

// Emulation architecture, refer to gamul.h
gamul8 gamer;

/* GLUT handling functions
 *  draw_square: method to draw a single pixel on the screen
 *  render: update and render logic, called by glutDisplayFunc
 *  idle: idling function for animation, called by glutIdleFunc
 *  initGL: initializes GL 2D mode and other settings
 *  key_pressed: function to handle user keyboard input (when pressed)
 *  key_release: function to handle key release
 */
void draw_square(float x_coord, float y_coord);
void render();
void idle();
void initGL();
void key_pressed(unsigned char key, int x, int y);
void key_release(unsigned char key, int x, int y);



/*	FUNCTION: main
 *  --------------
 *	Main emulation loop. Loads ROM, executes it, and draws to screen.
 *	PARAMETERS:
 *				argv: number of command line arguments
 *				argc[]: array of command line arguments
 *	RETURNS:	usually 0
 */
int main(int argc, char *argv[])
{
  /* Setup emulation system
   *  Check for filename argument
   *  Allocate system state and status variables
   *  Initalize system memory and program counter
   */
  if (argc < 2) return -1;
  trace_aloc();
  char *file = argv[1];
  load_file(file);

  /* Setup GLUT
   *  Seed random variable for use in emulation
   *  Initialize GLUT
   *  Initialize display and window
   *  Initialize orthographic 2D view, among other things
   */
  srand(time(NULL));
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("ESE 350: Lab 4 Gamul8r");
  initGL();

  /* Define action handlers
   *  Handle key presses and releases
   *  GLUT draw function
   *  GLUT idle function, causes screen to redraw
   */
  glutKeyboardFunc(your_key_press_handler);
  glutKeyboardUpFunc(your_key_release_handler);
  glutDisplayFunc(render);
  glutIdleFunc(idle);

	// Begin emulation: all events processed here
	glutMainLoop();
	return 0;
}



/*	FUNCTION: draw_square
 *  ----------------------
 *	Draws a square. Represents a single pixel
 *  (Up-scaled to a 640 x 320 display for better visibility)
 *	PARAMETERS: 
 *	x_coord: x coordinate of the square
 *	y_coord: y coordinate of the square
 *	RETURNS: none
 */
void draw_square(float x_coord, float y_coord)
{
  /* Draw a 10x10 white square at the coords
   *  GL_QUADS: takes 4 vertices of an independent quadrilateral
   *  glColor3f: color of the square in RGB, values between 0 & 1
   *  glVertex2f: pass in X and Y coordinates of each vertex
   *  glVertex3f: (optional) pass in X, Y and Z to draw 3D
   */
  glBegin(GL_QUADS);
  glColor3f(1.0f, 1.0f, 1.0f);
  glVertex2f(x_coord, y_coord);
  glVertex2f(x_coord + 10, y_coord);
  glVertex2f(x_coord + 10, y_coord + 10);
  glVertex2f(x_coord, y_coord + 10);
  glEnd();
}



/*	FUNCTION: render
 *  ----------------
 *	GLUT render function to draw the display. Also emulates one
 *	cycle of emulation.
 *	PARAMETERS: none
 *	RETURNS: none
 */
void render()
{
  // Execute instructions
  trace_exec();

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display_func(&gamer);

	// Draw a pixel for each display bit
	int i, j;
	for (i = 0; i < SCREEN_WIDTH; i++) {
		for (j = 0; j < SCREEN_HEIGHT; j++) {
			if (gamer.display[i][j] == 1) {
				draw_square((float)(i * 10), (float)(j * 10));
			}
		}
	}

	// Swap buffers, allows for smooth animation
	glutSwapBuffers();
}



/*	FUNCTION: idle
 *  -------------- 
 *	GLUT idle function. Instructs GLUT window to redraw itself
 *	PARAMETERS: none
 *	RETURNS: none
 */
void idle()
{
	// Redraw the screen
	glutPostRedisplay();
}



/*	FUNCTION: initGL
 *  ----------------
 *	Initializes GLUT settings.
 *	PARAMETERS: none
 *	RETURN VALUE: none
 */
void initGL()
{
	// Setup a GLUT window for 2D drawing and clear the screen
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.f, 0.f, 0.f, 1.f);
}



/*	FUNCTION: key_pressed
 *  --------------------------------
 *	Handles all keypresses and passes them to the emulator.
 *  This is also where you will be mapping the original GAMUL8
 *  keyboard layout to the layout of your choice
 *  Something like this:
 *  |1|2|3|C|		=>		|1|2|3|4|
 *	|4|5|6|D|		=>		|Q|W|E|R|
 *	|7|8|9|E|		=>		|A|S|D|F|
 *	|A|0|B|F|		=>		|Z|X|C|V|
 *	PARAMETERS: 
 *	key: the key that is pressed.
 *	x: syntax required by GLUT
 *	y: syntax required by GLUT
 *  (x & y are callback parameters that indicate the mouse location
 *  on the window. We are not using the mouse, so they won't be used, 
 *  but still pass them as glutKeyboardFunc needs it.) 
 *	RETURNS: none
 *  NOTE: If you intend to handle this in a different manner, you need not
 *  write this function.
 */
void key_pressed(unsigned char key, int x, int y)
{
  //TODO
}



/*	FUNCTION: key_release
 *  --------------------------------
 *	Tells emulator if any key is released. You can maybe give
 *  a default value if the key is released.
 *	PARAMETERS: 
 *	key: the key that is pressed.
 *	x: syntax required by GLUT
 *	y: syntax required by GLUT
 *  (x & y are callback parameters that indicate the mouse location
 *  on the window. We are not using the mouse, so they won't be used, 
 *  but still pass them as glutKeyboardFunc needs it.) 
 *	RETURNS: none
 *  NOTE: If you intend to handle this in a different manner, you need not
 *  write this function.
 */
void key_release(unsigned char key, int x, int y)
{
  //TODO
}
