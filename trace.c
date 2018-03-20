/* FILE: trace.c
 * PURPOSE: execute gamul8 assembly instructions
 */

#include <stdio.h>
#include <stdlib.h>
#include "trace.h"

#define INDEX_0 0
#define INDEX_1 512
#define INDEX_2 4095



/*  FUNCTION: trace_code
 *  ----------------------
 *  Decodes and performs gamul8 assembly instructions
 *  PARAMETERS: none
 *  RETURNS: none
 */
void trace_code()
{
  unsigned short IN = memory[(int) PC];
  unsigned short C1 = (IN >> 12) & 0x000F;
  unsigned short C2 = (IN >> 8) & 0x000F;
  unsigned short C3 = (IN >> 4) & 0x000F;
  unsigned short C4 = IN & 0x000F;
  int inc = 1;

  // 00E0: Clear the screen
  if (IN == 0x00E0) {
    //TODO
  }

  // 00EE: Return from a subroutine
  else if (IN == 0x00EE) {
    PC = stack[(int) spntr];
    --spntr;
    inc = 0;
  }

  // 1NNN: Jump to address at NNN
  else if (C1 == 1) {
    PC = IN & 0x0FFF;
    inc = 0;
  }

  // 1NNN: Calls subroutine at NNN
  else if (C1 == 2) {
    stack[(int) spntr] = PC + 1;
    ++spntr;
    PC = IN & 0x0FFF;
    inc = 0;
  }

  // 3XNN: Skip the next instruction if VX = NN
  else if (C1 == 3) {
    if (reg_Vx[(int) C2] == (IN & 0x00FF)) ++PC;
  }

  // 4NNN: Skip the next instruction if VX != NN
  else if (C1 == 4) {
    if (reg_Vx[(int) C2] != (IN & 0x00FF)) ++PC;
  }

  // 5XY0: Skip the next instruction if VX = VY
  else if ((C1 == 5) && (C4 == 0)) {
    if (reg_Vx[(int) C2] == reg_Vx[(int) C3]) ++PC;
  }

  // 6XNN: Set VX to NN
  else if (C1 == 6) {
    reg_Vx[(int) C2] = (IN & 0x00FF);
  }

  // 7XNN: Add NN to VX
  else if (C1 == 7) {
    reg_Vx[(int) C2] += (IN & 0x00FF);
  }

  /* 8XYZ: Assignment, Bitwise Operations, and Math
   *  8XY0: VX = VY
   *  8XY1: VX |= VY
   *  8XY2: VX &= VY
   *  8XY3: VX ^= VY
   *  8XY4: VX += VY
   *  8XY5: VX -= VY
   *  8XY6: VX >>= 1
   *  8XY7: VX = VY - VX
   *  8XYE: VX <<= 1
   */
  else if (C1 == 8) {
    if (C4 == 0) reg_Vx[(int) C2] = reg_Vx[(int) VY];
    else if (C4 == 1) reg_Vx[(int) C2] |= reg_Vx[(int) VY];
    else if (C4 == 2) reg_Vx[(int) C2] &= reg_Vx[(int) VY];
    else if (C4 == 3) reg_Vx[(int) C2] ^= reg_Vx[(int) VY];
    else if (C4 == 4) {
      reg_Vx[(int) C2] += reg_Vx[(int) VY];
      //TODO VF = 1 if carry else 0
    }
    else if (C4 == 5) {
      reg_Vx[(int) C2] -= reg_Vx[(int) VY];
      //TODO VF = 1 if no borrow else 0
    }
    else if (C4 == 6) {
      reg_Vx[(int) C2] >>= 1;
      //TODO VF = MSB of VX before shift
    }
    else if (C4 == 7) {
      reg_Vx[(int) C2] = reg_Vx[(int) VY] - reg_Vx[(int) C2];
      //TODO VF = 1 if no borrow else 0
    }
    else if (C4 == 14) {
      reg_Vx[(int) C2] <<= 1;
      //TODO VF = MSB of VX before shift
    }

    // Illegal Instruction
    else {
      printf("ERROR: ILLEGAL INSTRUCTION AT %d\n", &PC);
      trace_quit();
    }
  }

  // 9XNN: Skip the next instruction if VX != VY
  else if ((C1 == 9) && (C4 == 0)) {
    if (reg_Vx[(int) C2] != reg_Vx[(int) C3]) ++PC;
  }

  // ANNN: Set ID_reg to NNN
  else if (C1 == 10) {
    ID_reg = (IN & 0x0FFF);
  }

  // BNNN: Jump to NNN + V0
  else if (C1 == 11) {
    PC = (IN & 0x0FFF) + reg_Vx[0];
    inc = 0;
  }

  // CXNN: Set VC to a bitwise operation between NN and a random variable
  else if (C1 == 12) {
    //TODO
  }

  // DXYN: Display
  else if (C1 == 13) {
    //TODO
  }

  // EX9E: Skip the next instruction if key at VX = 1
  else if ((C1 == 14) && ((IN & 0x00FF) == 0x009E)) {
    if (keypad[(int) reg_Vx[(int) C2]] == 1) ++PC;
  }

  // EXA1: Skip the next instruction if key at VX = 0
  else if ((C1 == 14) && ((IN & 0x00FF) == 0x009E)) {
    if (keypad[(int) reg_Vx[(int) C2]] == 0) ++PC;
  }

  // FX07: Set VX = delay
  else if ((C1 == 15) && ((IN & 0x0FF) == 0x0007)) {
    reg_Vx[(int) C2] = delay;
  }

  // FX0A: Waits for key press then stores in VX
  else if ((C1 == 15) && ((IN & 0x0FF) == 0x000A)) {
    //TODO
  }

  // FX15: Set delay = VX
  else if ((C1 == 15) && ((IN & 0x0FF) == 0x0015)) {
    delay = reg_Vx[(int) C2];
  }

  // FX18: Set sound = VX
  else if ((C1 == 15) && ((IN & 0x0FF) == 0x0018)) {
    sound = reg_Vx[(int) C2];
  }

  // FX1E: Set ID_reg += VX
  else if ((C1 == 15) && ((IN & 0x0FF) == 0x001E)) {
    ID_reg += reg_Vx[(int) C2];
  }

  // FX29: Set ID_reg to addr of sprite at VX
  else if ((C1 == 15) && ((IN & 0x0FF) == 0x0029)) {
    //TODO
  }

  // FX33: TODO
  else if ((C1 == 15) && ((IN & 0x0FF) == 0x0033)) {
    //TODO
  }

  // FX55: Store VX:V0 to memory starting at ID_reg
  else if ((C1 == 15) && ((IN & 0x0FF) == 0x0055)) {
    //TODO
  }

  // FX65: Load VX:V0 from memory starting at ID_reg
  else if ((C1 == 15) && ((IN & 0x0FF) == 0x0065)) {
    //TODO
  }

  // Illegal Instruction
  else {
    printf("ERROR: ILLEGAL INSTRUCTION AT %d\n", &PC);
    trace_quit();
  }

  // Increment the program
  if (inc == 1) ++PC;
}



/*  FUNCTION: trace_exec
 *  ----------------------
 *  Executes one cycle of instruction tracing
 *  PARAMETERS: none
 *  RETURNS: none
 */
void trace_exec()
{
  //TODO: Chnage the logic of this

  // Execute the trace
  while (1) {
    R8 = reg[8];
    IN = &mem[(int) R8];
    trace_code (mem, reg);

    // Handle Exception 1
    if (((R8 >= INDEX_1) && (R8 < INDEX_2)) || (R8 >= INDEX_3)) {
      printf ("ERROR: Attempting to execute a data section address as code.\n");
      break;
    }

    // Handle Exception 2
    if ((int) reg[10] == 0x0002) {
      printf ("ERROR: Attempting to read or write a code section as data.\n");
      break;
    }

    // Handle Exception 3
    if ((int) reg[10] == 0x0003) {
      printf ("ERROR: Attempting to access the OS section of memory with insufficient permissions.\n");
      break;
    }

    // Write output
    fwrite (PC, 2, 1, outfile);
    fwrite (IN, 2, 1, outfile);

    // Handle end
    if ((int) reg[8] == 0x80FF) {
      fwrite (&reg[8], 2, 1, outfile);
      fwrite (&mem[(int) reg[8]], 2, 1, outfile);
      break;
    }
  }
  return;
}



/*  FUNCTION: trace_aloc
 *  ----------------------
 *  Allocates system state and status.
 *  PARAMETERS: none
 *  RETURNS: none
 */
void trace_aloc()
{
  memory = calloc(MEM_SIZE, 2);
  if (memory == NULL) {
    printf("ERROR: INSUFFICIENT MEMORY\n");
    exit(-1);
  }

  reg_Vx = calloc(REG_SIZE, 1);
  if (reg_Vx == NULL) {
    printf("ERROR: INSUFFICIENT MEMORY\n");
    free(memory);
    exit(-1);
  }

  ID_reg = calloc(1, 2);
  if (ID_reg == NULL) {
    printf("ERROR: INSUFFICIENT MEMORY\n");
    free(memory);
    free(reg_Vx);
    exit(-1);
  }

  keypad = calloc(1, 2);
  if (keypad == NULL) {
    printf("ERROR: INSUFFICIENT MEMORY\n");
    free(memory);
    free(reg_Vx);
    free(ID_reg);
    exit(-1);
  }

  stack = calloc(16, 2);
  if (stack == NULL) {
    printf("ERROR: INSUFFICIENT MEMORY\n");
    free(memory);
    free(reg_Vx);
    free(ID_reg);
    free(keypad);
    exit(-1);
  }

  spntr = calloc(1, 1);
  if (spntr == NULL) {
    printf("ERROR: INSUFFICIENT MEMORY\n");
    free(memory);
    free(reg_Vx);
    free(ID_reg);
    free(keypad);
    free(stack);
    exit(-1);
  }

  sound = calloc(1, 1);
  if (sound == NULL) {
    printf("ERROR: INSUFFICIENT MEMORY\n");
    free(memory);
    free(reg_Vx);
    free(ID_reg);
    free(keypad);
    free(stack);
    free(spntr);
    exit(-1);
  }

  delay = calloc(1, 1);
  if (delay == NULL) {
    printf("ERROR: INSUFFICIENT MEMORY\n");
    free(memory);
    free(reg_Vx);
    free(ID_reg);
    free(keypad);
    free(stack);
    free(spntr);
    free(sound);
    exit(-1);
  }

  PC = calloc(1, 2);
  if (PC == NULL) {
    printf("ERROR: INSUFFICIENT MEMORY\n");
    free(memory);
    free(reg_Vx);
    free(ID_reg);
    free(keypad);
    free(stack);
    free(spntr);
    free(sound);
    free(delay);
    exit(-1);
  }
}



/*  FUNCTION: trace_quit
 *  ----------------------
 *  Frees all allocated memory.
 *  PARAMETERS: none
 *  RETURNS: none
 */
void trace_quit()
{
  free(memory);
  free(reg_Vx);
  free(ID_reg);
  free(keypad);
  free(stack);
  free(spntr);
  free(sound);
  free(delay);
  free(PC);
  exit(-1);
}
