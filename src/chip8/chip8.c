#include "chip8.h"
#include <stdio.h> 
#include <stdlib.h> // rand
#include <stdint.h> 
#include <stdbool.h> 
#include <time.h> 
#include <string.h> // memset and memcpy 

// chip8 fontset 
const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(chip8_t *chip8){
   memset(chip8,0,sizeof(chip8_t));

   // load font 
   memcpy(&chip8->memory[0],fontset,sizeof(fontset));

   // set program counter 
   chip8->pc = 0x200;
   chip8->draw_flag = true;

   // Seed random number generator
   srand(time(NULL));
}

bool chip8_load_rom(chip8_t *chip8,const char *filename){

   // open file 
   FILE *file = fopen(filename, "rb");
   if(!file){
      fprintf(stderr, "Cannot open %s file\n",filename);
      return false;
   }

   fseek(file,0,SEEK_END);
   long size = ftell(file);
   rewind(file);

   // check if file is big
   if(size > MEMORY - 0x200){
      fprintf(stderr, "ROM too big: %ld bytes\n",size);
      fclose(file);
      return false;
   }
   
   // read into the memory
   if (!fread(&chip8->memory[0x200],1,size,file)) {
      fprintf(stderr, "Cant read %s into memory!\n",filename);
      return false;
   }

   fclose(file);
   return true; 
}

void chip8_timers(chip8_t *chip8){
   if (chip8->delay_timer > 0) { 
      chip8->delay_timer--;
   }
   if (chip8->sound_timer > 0) { 
      if (chip8->sound_timer == 1) { 
         printf("kela!\n");
      }
      chip8->sound_timer--;
   }
}



void chip8_cycle(chip8_t *chip8){
   /* Bitwise operation & bit masking
    chip8->memory[chip8->pc] << 8 = 0x12 << 8 = 0x1200
    chip8->memory[chip8->pc+1] = 0x34
    0x1200 | 0x34 = 0x1234 ← The complete 16-bit opcode!

    CHIP-8 is big-endian: most significant byte comes first in memory
    memory[pc] is the high byte, memory[pc+1] is the low byte
    The shift and OR preserves this ordering correctly
   */
   uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc+1];
   chip8->pc += 2;

   uint16_t pc_before = chip8->pc - 2;  // Address where this opcode was fetched
   printf("\n PC:[0x%04X] OP:0x%04X ASM:", pc_before, opcode);

   // Decode opcode
   // now we need to decode the opcoded 16 breaking down into bits and bytes
   // Bit MASKING
   /*
        Opcode:    1101 0010 1011 1100  (0xD2BC)
        Mask:      0000 1111 1111 1111  (0x0FFF)
        Result:    0000 0010 1011 1100  (0x02BC = nnn)

        Opcode:    1101 0010 1011 1100  (0xD2BC)
        Mask:      0000 1111 0000 0000  (0x0F00)
        Result:    0000 0010 0000 0000  (0x0200)
        Shift >>8: 0000 0000 0000 0010  (0x02 = x)
   */

   uint8_t first_nibble = (opcode & 0xF000) >> 12;  // 1111 0000 0000 0000             
   uint8_t x = (opcode & 0x0F00) >> 8;              // 0000 1111 0000 0000             
   uint8_t y = (opcode & 0x00F0) >> 4;              // 0000 0000 1111 0000
   uint8_t n = opcode & 0x000F;                     // 0000 0000 0000 1111
   uint8_t kk = opcode & 0x00FF;                    // 0000 0000 1111 1111
   uint16_t nnn = opcode & 0x0FFF;                  // 0000 1111 1111 1111
   // opcode                                        // 1111 1111 1111 1111

   switch (first_nibble) {
      case 0x0:
         if (kk == 0xE0) { // Clear Screen 
            printf("CLS (Clear Screen)");
            memset(chip8->display, 0, sizeof(chip8->display));
            chip8->draw_flag = true;
            break;
         } else if (kk == 0xEE) { // Return from subroutine
            printf("RET (Return from subroutine)");
            chip8->sp--;
            chip8->pc = chip8->stack[chip8->sp];
            break;
         } else {
             printf("SYS 0x%03X (ignore)", nnn); // Backward compatiblity
         }
      break;  // 0x0 end

      case 0x1: // 1nnn jump to address
         /*
             opcode = 0x1234
             first_nibble = 0x1
             nnn = 0x234  // Jump target address
             
             // Execution:
             chip8->pc = 0x234;  // Program continues from address 0x234
         */
         printf("JP 0x%03X (Jump to add 0x%3X)",nnn,nnn);
         chip8->pc = nnn;
      break; // 0x1 end
      
      case 0x2: // 2nnn - Call subroutine at nnn
         /*
               Calls a subroutine at address nnn
               Saves return address on the stack before jumping
               Like a function call in high-level languages
         */
         printf("CALL 0x%03X (Call Subroutine at 0x%03X)", nnn,nnn);
         chip8->stack[chip8->sp] = chip8->pc; //save return address
         chip8->sp++;                         // move stack pointer up
         chip8->pc = nnn;                     // jump to subroutines

      break; // 0x2 end

      case 0x3: // 3xkk - Skip if Vx == kk
         /*
              Compares register Vx with immediate value kk
              Skips next instruction if they are equal
              Conditional skip - doesn't always execute
            */
         printf("SE V%X, 0x%02X (Skip V[%X]==kk[0x%02X])", x, kk , x , kk);
         if(chip8->V[x] == kk) chip8->pc += 2;
      break; // 0x3 end
      
   } // switch end 

}
