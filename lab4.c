// LAB 4

#define MEM_SIZE 4095
#define REG_SIZE 16

int main (void)
{
  //TODO: allocate [] for mem -> 4096 bytes (0xFFF : 0x000), first 512 bytes reserved for interpreter data, such as character and letter sprites (0x200 : 0x000)
  int mem*;

  //TODO: allocate [] for reg -> 16 general purpose 8-bit registers (Vx -> VF : V0), VF is reserved for instruction flag
  int reg*;

  //TODO: allocate index register -> 16-bit index register (I), used to store mem addrs for loading from/storing to mem -> (0xFFF : 0x000)
  int index_register*;

  //TODO: allocate PC -> 16 bit, sote the currently executing addr (0xFFF : 0x000)
  int PC*;

  //TODO: allocate stack -> 16 16-bit values, used to store addr that interpreter should ret to when finished w/a subroutine; Stack Pointer (SP) is 8-bit and points to topmost mem loc of the stack
  int stack*;

  //TODO: allocate sound and delay -> both 8-bit; when nonzero, decrement at a rate of 60Hz
  int av_io*;

  //TODO: allocate display -> each pixel is ON of OFF; 2048 pixels (64 x 32)
  int disp*;

  //TODO: allocate keypad -> 16 keys (0xF : 0x0)
  int keys*;
}
