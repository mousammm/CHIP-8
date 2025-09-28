#include "chip8.h"
#include <stdio.h> // printf
#include <stdlib.h> // rand
#include <stdint.h> // uint8
#include <stdbool.h> //bool
#include <time.h> // time 
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
   FILE *file = fopen(filename, "rb");

   if(!file){
      fprintf(stderr, "Cannot open %s file\n",filename);
      return false;
   }

   fseek(file,0,SEEK_END);
   long size = ftell(file);
   rewind(file);

   if(size > MEMORY - 0x200){
      fprintf(stderr, "ROM too big: %ld bytes\n",size);
      fclose(file);
      return false;
   }
   
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
   uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc+1];
   chip8->pc += 2;

   // Decode opcode
   uint16_t nnn = opcode & 0x0FFF;
   uint8_t n = opcode & 0x000F;
   uint8_t x = (opcode & 0x0F00) >> 8;
   uint8_t y = (opcode & 0x00F0) >> 4;
   uint8_t kk = opcode & 0x00FF;

   // Execute opcode
   switch(opcode & 0xF000) {
      case 0x0000:
         switch(opcode) {
            case 0x00E0: // Clear screen
               memset(chip8->display, 0, sizeof(chip8->display));
               chip8->draw_flag = true;
               break;
            case 0x00EE: // Return from subroutine
               chip8->sp--;
               chip8->pc = chip8->stack[chip8->sp];
               break;
         }
         break;
         
      case 0x1000: // 1nnn - Jump to address nnn
         chip8->pc = nnn;
         break;
         
      case 0x2000: // 2nnn - Call subroutine at nnn
         chip8->stack[chip8->sp] = chip8->pc;
         chip8->sp++;
         chip8->pc = nnn;
         break;
         
      case 0x3000: // 3xkk - Skip if Vx == kk
         if(chip8->V[x] == kk) chip8->pc += 2;
         break;
         
      case 0x4000: // 4xkk - Skip if Vx != kk
         if(chip8->V[x] != kk) chip8->pc += 2;
         break;
         
      case 0x5000: // 5xy0 - Skip if Vx == Vy
         if(chip8->V[x] == chip8->V[y]) chip8->pc += 2;
         break;
         
      case 0x6000: // 6xkk - Set Vx = kk
         chip8->V[x] = kk;
         break;
         
      case 0x7000: // 7xkk - Add kk to Vx
         chip8->V[x] += kk;
         break;
         
      case 0x8000:
         switch(n) {
            case 0x0: // 8xy0 - Set Vx = Vy
               chip8->V[x] = chip8->V[y];
               break;
            case 0x1: // 8xy1 - Set Vx = Vx OR Vy
               chip8->V[x] |= chip8->V[y];
               break;
            case 0x2: // 8xy2 - Set Vx = Vx AND Vy
               chip8->V[x] &= chip8->V[y];
               break;
            case 0x3: // 8xy3 - Set Vx = Vx XOR Vy
               chip8->V[x] ^= chip8->V[y];
               break;
            case 0x4: // 8xy4 - Add Vy to Vx, set VF on carry
               {
                  uint16_t sum = chip8->V[x] + chip8->V[y];
                  chip8->V[0xF] = (sum > 0xFF) ? 1 : 0;
                  chip8->V[x] = sum & 0xFF;
               }
               break;
            case 0x5: // 8xy5 - Subtract Vy from Vx, set VF on NOT borrow
               chip8->V[0xF] = (chip8->V[x] > chip8->V[y]) ? 1 : 0;
               chip8->V[x] -= chip8->V[y];
               break;
            case 0x6: // 8xy6 - Shift Vx right by 1, set VF to LSB
               chip8->V[0xF] = chip8->V[x] & 0x1;
               chip8->V[x] >>= 1;
               break;
            case 0x7: // 8xy7 - Set Vx = Vy - Vx, set VF on NOT borrow
               chip8->V[0xF] = (chip8->V[y] > chip8->V[x]) ? 1 : 0;
               chip8->V[x] = chip8->V[y] - chip8->V[x];
               break;
            case 0xE: // 8xyE - Shift Vx left by 1, set VF to MSB
               chip8->V[0xF] = (chip8->V[x] & 0x80) >> 7;
               chip8->V[x] <<= 1;
               break;
         }
         break;
         
      case 0x9000: // 9xy0 - Skip if Vx != Vy
         if(chip8->V[x] != chip8->V[y]) chip8->pc += 2;
         break;
         
      case 0xA000: // Annn - Set I = nnn
         chip8->I = nnn;
         break;
         
      case 0xB000: // Bnnn - Jump to nnn + V0
         chip8->pc = nnn + chip8->V[0];
         break;
         
      case 0xC000: // Cxkk - Set Vx = random byte AND kk
         chip8->V[x] = (rand() % 256) & kk;
         break;
         
      case 0xD000: // Dxyn - Draw sprite at (Vx, Vy) with n bytes of sprite data
         {
            uint8_t x_pos = chip8->V[x] % DISPLAY_WIDTH;
            uint8_t y_pos = chip8->V[y] % DISPLAY_HEIGHT;
            chip8->V[0xF] = 0;
            
            for(int row = 0; row < n; row++) {
               uint8_t sprite_byte = chip8->memory[chip8->I + row];
               
               for(int col = 0; col < 8; col++) {
                  if((sprite_byte & (0x80 >> col)) != 0) {
                     int pixel_x = (x_pos + col) % DISPLAY_WIDTH;
                     int pixel_y = (y_pos + row) % DISPLAY_HEIGHT;
                     int pixel_index = pixel_y * DISPLAY_WIDTH + pixel_x;
                     
                     if(chip8->display[pixel_index] == 1) {
                        chip8->V[0xF] = 1;
                     }
                     chip8->display[pixel_index] ^= 1;
                  }
               }
            }
            chip8->draw_flag = true;
         }
         break;
         
      case 0xE000:
         switch(kk) {
            case 0x9E: // Ex9E - Skip if key in Vx is pressed
               if(chip8->keypad[chip8->V[x]]) chip8->pc += 2;
               break;
            case 0xA1: // ExA1 - Skip if key in Vx is NOT pressed
               if(!chip8->keypad[chip8->V[x]]) chip8->pc += 2;
               break;
         }
         break;
         
      case 0xF000:
         switch(kk) {
            case 0x07: // Fx07 - Set Vx = delay timer
               chip8->V[x] = chip8->delay_timer;
               break;
            case 0x0A: // Fx0A - Wait for key press, store in Vx
               {
                  bool key_pressed = false;
                  for(int i = 0; i < 16; i++) {
                     if(chip8->keypad[i]) {
                        chip8->V[x] = i;
                        key_pressed = true;
                        break;
                     }
                  }
                  if(!key_pressed) chip8->pc -= 2; // Try again next cycle
               }
               break;
            case 0x15: // Fx15 - Set delay timer = Vx
               chip8->delay_timer = chip8->V[x];
               break;
            case 0x18: // Fx18 - Set sound timer = Vx
               chip8->sound_timer = chip8->V[x];
               break;
            case 0x1E: // Fx1E - Set I = I + Vx
               chip8->I += chip8->V[x];
               break;
            case 0x29: // Fx29 - Set I to location of sprite for digit Vx
               chip8->I = chip8->V[x] * 5; // Each sprite is 5 bytes
               break;
            case 0x33: // Fx33 - Store BCD representation of Vx in memory
               chip8->memory[chip8->I] = chip8->V[x] / 100;
               chip8->memory[chip8->I + 1] = (chip8->V[x] / 10) % 10;
               chip8->memory[chip8->I + 2] = chip8->V[x] % 10;
               break;
            case 0x55: // Fx55 - Store V0 to Vx in memory starting at I
               for(int i = 0; i <= x; i++) {
                  chip8->memory[chip8->I + i] = chip8->V[i];
               }
               break;
            case 0x65: // Fx65 - Read V0 to Vx from memory starting at I
               for(int i = 0; i <= x; i++) {
                  chip8->V[i] = chip8->memory[chip8->I + i];
               }
               break;
         }
         break;
         
      default:
         printf("Unknown opcode: 0x%04X\n", opcode);
         break;
   }
}
