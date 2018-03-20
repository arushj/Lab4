#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gamul.h"

// font set for rendering
const unsigned char fontset[FONTSET_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
	0x20, 0x60, 0x20, 0x20, 0x70,		// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
	0xF0, 0x80, 0xF0, 0x80, 0x80		// F
};



/*	FUNCTION: load_file
 *  -------------------
 *	Loads the given program/game
 *	PARAMETERS: 
 *  filename: name of file to be loaded
 *	RETURNS: 0 if successful, -1 if file error
 */
int load_file(char *filename)
{
  /* Memory initialization
   *  Open the game file in binary read-only mode
   *  Load instructions into memory individually
   */
  FILE *file = fopen(filename, "rb");
  if (file == NULL){
    printf("ERROR: FILE NOT FOUND\n");
    return -1;
  }
  unsigned short nxt = 0x0;
  PC = 0x200;
  while (1) {
    if (PC > 0xFFF) {
      printf("ERROR: MEMORY OVERRUN")
      fclose(file);
      return -1;
    }
    nxt = fgetc(file);
    if (nxt == EOF) {
      fclose(file);
      PC = 0x200;
      return 0;
    }
    nxt = nxt << 8;
    nxt = nxt | fgetc(file);
    memory[(int) PC] = nxt;
    ++PC;
  }
}



/*	FUNCTION: display_func
 *  ----------------------
 *	Sample function that displays a pixel on the screen
 *	PARAMETERS: 
 *  gamer: architecture to be emulated, defined in gamul.h
 *	RETURNS: none
 */
void display_func(gamul8 *gamer)
{
	gamer->display[20][40] = 1;
	gamer->display[10][30] = 0;
}
