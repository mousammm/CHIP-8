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
         printf("SE V%X, 0x%02X (Skip next inst if V[%X]==kk[0x%02X])", x, kk , x , kk);
         if(chip8->V[x] == kk) chip8->pc += 2;
      break; // 0x3 end
      
      case 0x4: // 4xkk - Skip if Vx != kk (opposite of 0x3)
         printf("SNE V%X 0x%03X (Skip next ins if V[%X]!=kk[0x%03X])", x , kk , x , kk);
         if(chip8->V[x] != kk) chip8->pc += 2;
      break; // 0x4 end

      case 0x5: // 5xy0 - Skip if Vx == Vy
         printf("SE V%X, V%X (Skip next inst if V%X==y[V%X])", x, y , x , y);
         if(chip8->V[x] == chip8->V[y]) chip8->pc += 2;
      break; // 0x5 end 

      case 0x6: // 6xkk - LD Vx, byte (Load immediate value into register)
         printf("LD V%X, 0x%02X (Set V%X=kk[0x%2X])", x, kk , x , kk );
         chip8->V[x] = kk;
      break; // 0x6 end 
 
      case 0x7: // 7xkk - Add kk to Vx
         /*
             opcode = 0x7105
             first_nibble = 0x7
             x = 0x1      // Register V1
             kk = 0x05    // Value to add
             
             // Execution:
             chip8->V[1] += 0x05;  // Simple addition (no carry flag)
            */
         printf("ADD V%X, 0x%02X (Add V%X+=kk[0x%02X])", x, kk , x, kk);
         chip8->V[x] += kk;
      break; // 0x7 end
 
      case 0x8: // 0x8xxx - Arithmetic and Logic Operations
         /* 8xyn
            Where:
            - 8 = opcode identifier
            - x = first register index (0-F)
            - y = second register index (0-F)
            - n = operation type (0-E) */
         switch (n) {
            case 0x0: // 8xy0 - LD Vx, Vy
               printf("LD V%X, V%X (Load Vy into Vx)", x , y );
               chip8->V[x] = chip8->V[y];
            break; // 0x0 end 

            case 0x1: // 8xy1 - OR Vx, Vy
               printf("OR V%X, V%X (BITWISE OR )", x , y );
               chip8->V[x] |= chip8->V[y];
            break; // 0x1 end 

            case 0x2: // 8xy2 - AND Vx, Vy
               printf("AND V%X, V%X (BITWISE AND)", x , y );
               chip8->V[x] &= chip8->V[y];
            break; // 0x2 end 

            case 0x3: // 8xy3 - XOR Vx, Vy
               printf("XOR V%X, V%X (BITWISE XOR)", x , y );
               chip8->V[x] ^= chip8->V[y];
            break; // 0x3 end 

            case 0x4: // 8xy4 - ADD Vx, Vy (with carry)
               /* V1 + V2 = 0x80 + 0x90 = 0x110 (272 in decimal)
                  Result:
                  V1 = 0x10  // Only keep lower 8 bits
                  VF = 1     // Carry flag set because 0x110 > 0xFF
                  */
               printf("ADD V%X, V%X (ADD with CARRY)", x , y );
               uint16_t sum = chip8->V[x] + chip8->V[y];
               chip8->V[x] = sum & 0xFF;
               chip8->V[0xF] = (sum > 0xFF);
            break; // 0x4 end 

            case 0x5: // 8xy5 - SUB Vx, Vy 
               /*0x90 - 0x80 = 0x10
                 Result:
                 V1 = 0x10
                 VF = 1     // No borrow needed (V1 > V2) */
               printf("SUB V%X, V%X (SUBTRACT)", x , y );
               chip8->V[0xF] = (chip8->V[x] > chip8->V[y]);
               chip8->V[x] -= chip8->V[y];
            break; // 0x5 end 

            case 0x6: // 8xy6 - SHR Vx, Vy 
               /* Shift V1 right: 1000 0101 → 0100 0010 (0x42)
                  Least significant bit was 1
                  Result:
                  V1 = 0x42
                  VF = 1     // Carry = bit that was shifted out  */
               printf("SHR V%X, V%X (SHIFT RIGHT)", x , y );
               chip8->V[0xF] = chip8->V[x] &  0x1; // set carry flag to least significant bit 
               chip8->V[x] >>= 1;                  // shift right by 1
            break; // 0x6 end 

            case 0x7:// 8xy7 - SUB Vx, Vy 
               /* V2 - V1 = 0x30 - 0x20 = 0x10
                  Result:
                  V1 = 0x10  // Result stored in V1
                  VF = 1     // No borrow (V2 > V1) */
               printf("SUBN V%X, V%X (SUBTRACT REVERSED)", x , y );
               chip8->V[0xF] = (chip8->V[y] > chip8->V[x]);
               chip8->V[x] = chip8->V[y] - chip8->V[x];
            break; // 0x7 end 

            case 0xE:// 8xyE - SHL Vx, Vy
               /* Shift V1 left: 1100 0001 → 1000 0010 (0x82)
                  Most significant bit was 1
                  Result:
                  V1 = 0x82
                  VF = 1     // Carry = bit that was shifted out */
               printf("SHL V%X, V%X (SHIFT LEFT)", x , y );
               chip8->V[0xF] = (chip8->V[x] &  0x80) >> 7; // set carry flag to most significant bit 
               chip8->V[x] <<= 1;                          // shift left by 1
            break; // 0xE end 

         } // switch(n) end 
      break; // 0x8 end

      case 0x9: // 9xy0 - SNE Vx, Vy Skip if Vx != Vy
         printf("SNE V%X, V%X (SKIP IF VX != VY)", x, y);
         if(chip8->V[x] != chip8->V[y]) chip8->pc += 2;
      break; // 0x9
       
      case 0xA: // Annn - LD I, addr (Load Index Register with address)
         /*     Loads a 12-bit address into the index register I
               - A = opcode identifier (0xA)
               - nnn = 12-bit address to load into I */
         printf("LD I, 0x%03X (LOADS 12bit into I)", nnn);
         chip8->I = nnn;
      break; // 0xA end 

      case 0xB: // Bnnn - JP V0, addr (Jump to address V0 + nnn)
        printf("JP V0, 0x%03X (JUMP TO ADDr V0 + nnn)", nnn);
        chip8->pc = nnn + chip8->V[0];
      break; // 0xB end
        
      case 0xC: // Cxkk - RND Vx, byte (Random number generation)
         /* Cxkk
            Where:
            - C = opcode identifier (0xC)
            - x = register index (0-F)
            - kk = 8-bit mask value */
         printf("RND V%X, 0x%02X (RANDOM NO MASK WITH KK)", x, kk);
         chip8->V[x] = (rand() % 256) & kk;
      break; // 0xC end

      case 0xD: // Dxyn - Draw sprite at (Vx, Vy) with n bytes of sprite data
         /* - D = opcode identifier (0xD)
              - x = X coordinate register index
              - y = Y coordinate register index  
              - n = sprite height in pixels (1-15) */
        {
           printf("DRW V%X, V%X, %d (Draw X[%X] Y[%X] HE[%d])", x, y, n , x, y, n);

           uint8_t x_pos = chip8->V[x] % DISPLAY_WIDTH; // X cordinate (wrapped) x_pos = 70 % 64 = 6
           uint8_t y_pos = chip8->V[y] % DISPLAY_HEIGHT;// Y cordinate (wrapped) y_pos = 40 % 32 = 8
           chip8->V[0xF] = 0; // Reset colision flag 
           
           for(int row = 0; row < n; row++) { // process each sprite row 
               //reads the sprite data from memory, one row at a time
              uint8_t sprite_byte = chip8->memory[chip8->I + row];
              
              for(int col = 0; col < 8; col++) {  // process each pixel in row
                  // Bit Extraction: (0x80 >> col) scans bits from left to right (MSB first)
                  /* sprite_byte = 0xA5 (1010 0101 binary)
                     col=0: 0x80 >> 0 = 1000 0000 → checks bit 7 (1) ✓ DRAW
                     col=1: 0x80 >> 1 = 0100 0000 → checks bit 6 (0) ✗ skip  
                     col=2: 0x80 >> 2 = 0010 0000 → checks bit 5 (1) ✓ DRAW */
                 if((sprite_byte & (0x80 >> col)) != 0) { 
                     // calculate the exact screen position for each sprite pixel
                    int pixel_x = (x_pos + col) % DISPLAY_WIDTH;
                    int pixel_y = (y_pos + row) % DISPLAY_HEIGHT;
                     // converts (X,Y) coordinates into a 1D array index
                    int pixel_index = pixel_y * DISPLAY_WIDTH + pixel_x;
                    
                    if(chip8->display[pixel_index] == 1) {
                       chip8->V[0xF] = 1; // Collision detection
                    }
                    chip8->display[pixel_index] ^= 1;
                 }
              }
           }
           chip8->draw_flag = true;
        }
      break; // 0xD ends 

      case 0xE: // Keyboard Input Instructions
        switch(kk) {
           case 0x9E: // Ex9E - Skip if key in Vx is pressed
              printf("SKP V%X (skip Vx if key is press)", x);
              if(chip8->keypad[chip8->V[x]]) chip8->pc += 2;
              break;
           case 0xA1: // ExA1 - Skip if key in Vx is NOT pressed
              printf("SKNP V%X (Skip Vx if key not press)", x);
              if(!chip8->keypad[chip8->V[x]]) chip8->pc += 2;
              break;
        }
      break; // 0xE end
 
   } // switch end 

}
