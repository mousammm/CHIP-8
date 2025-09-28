#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>


//chip8 define
typedef struct  {
  uint8_t memory[4096];   //4kb ram/memory
  uint8_t V[16];           // 16 8-bit register 
  uint16_t I;               // Index register 
  uint16_t pc;            // Program counter
  uint8_t sp;             // stack pointer
  uint16_t stack[16];     // stack
  uint8_t delay_timer;    // delay timer
  uint8_t sound_timer;    // sound timer
  uint8_t display[64*32]; // 64*32 pixel display
  bool keypad[16];  // keypad state [0-F]
  bool draw_flag;         // redraw flag 
} chip8_t ;

 
//chip8 function declaration 
void chip8_init(chip8_t *chip8);
bool chip8_load_rom(chip8_t *chip8, const char *filename);
void chip8_cycle(chip8_t *chip8);
void chip8_timers(chip8_t *chip8);


#endif // CHIP8_H
